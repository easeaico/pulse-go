# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import sys
import logging
from pathlib import Path
from typing import Dict, List, Optional, Set


_pulse_logger = logging.getLogger('pulse')

# Translation dictionaries for table alignment
left_rule = {'<': ':', '^': ':', '>': '-'}
right_rule = {'<': '-', '^': ':', '>': ':'}


def evaluate_field(record, field_spec):
    """
    Evaluate a field of a record using the type of the field_spec as a guide.
    """
    if type(field_spec) is int:
        return str(record[field_spec])
    elif type(field_spec) is str:
        return str(getattr(record, field_spec))
    else:
        return str(field_spec(record))


def table(file, records, fields, headings, alignment = None):
    """
    Generate a Doxygen-flavor Markdown table from records.

    file -- Any object with a 'write' method that takes a single string
        parameter.
    records -- Iterable.  Rows will be generated from this.
    fields -- List of fields for each row.  Each entry may be an integer,
        string or a function.  If the entry is an integer, it is assumed to be
        an index of each record.  If the entry is a string, it is assumed to be
        a field of each record.  If the entry is a function, it is called with
        the record and its return value is taken as the value of the field.
    headings -- List of column headings.
    alignment - List of pairs alignment characters.  The first of the pair
        specifies the alignment of the header, (Doxygen won't respect this, but
        it might look good, the second specifies the alignment of the cells in
        the column.

        Possible alignment characters are:
            '<' = Left align (default for cells)
            '>' = Right align
            '^' = Center (default for column headings)

    References:
    http://www.doxygen.nl/manual/markdown.html#md_tables
    https://stackoverflow.com/questions/13394140/generate-markdown-tables
    """

    num_columns = len(fields)
    assert len(headings) == num_columns

    # Compute the table cell data
    columns = [[] for i in range(num_columns)]
    for record in records:
        for i, field in enumerate(fields):
            columns[i].append(evaluate_field(record, field))

    # Fill out any missing alignment characters.
    extended_align = alignment if alignment != None else []
    if len(extended_align) > num_columns:
        extended_align = extended_align[0:num_columns]
    elif len(extended_align) < num_columns:
        extended_align += [('^', '<') \
                           for i in range(num_columns-len(extended_align))]

    heading_align, cell_align = [x for x in zip(*extended_align)]

    field_widths = [len(max(column, key=len)) if len(column) > 0 else 0
                    for column in columns]
    heading_widths = [max(len(head), 2) for head in headings]
    column_widths = [max(x) for x in zip(field_widths, heading_widths)]

    _ = ' | '.join(['{:' + a + str(w) + '}'
                    for a, w in zip(heading_align, column_widths)])
    heading_template = '| ' + _ + ' |'
    _ = ' | '.join(['{:' + a + str(w) + '}'
                    for a, w in zip(cell_align, column_widths)])
    row_template = '| ' + _ + ' |'

    _ = ' | '.join([left_rule[a] + '-'*(w-2) + right_rule[a]
                    for a, w in zip(cell_align, column_widths)])
    ruling = '| ' + _ + ' |'

    file.write(heading_template.format(*headings).rstrip() + '\n')
    file.write(ruling.rstrip() + '\n')
    for row in zip(*columns):
        file.write(row_template.format(*row).rstrip() + '\n')

def get_table_tag(word: str)->str:
    start = word.find('{')
    end = word.find('}')
    if start == -1 or end == -1:
        _pulse_logger.warning(f"References cannot contain whitespace: {word}")
        exit(1)
    word = word[start:end + 1]
    return word


def process_file(fpath: Path, ref_dir: Path, dest_dir: Path,
                 replace_refs: bool = False, ancestors: Optional[Set[Path]] = None) -> None:
    """
    Prepares given file for doxygen. Inserts provided tables/references to other MD
    documents and optionally replaces references with Figure/Table numbers.
    """
    if ancestors is None:
        ancestors = set()

    out_fname = dest_dir.resolve() / fpath.stem
    out_fname = out_fname.with_suffix(out_fname.suffix+".md")
    out_fname.parent.mkdir(parents=True, exist_ok=True)

    def _process_references(lines: List[str]) -> List[str]:
        table_refs = {}
        fig_refs = {}
        eq_refs = {}

        # Build up references
        def _parse_refs(line: str, tag: str) -> List[str]:
            ref_defs = []
            words = line.split()

            for idx, word in enumerate(words):
                if tag in word:
                    ref_name = get_table_tag(words[idx+1])
                    ref_defs.append(ref_name)

            return ref_defs

        table_num = 1
        fig_num = 1
        eq_num = 1
        for line in lines:
            if "@tabledef" in line:
                for ref_def in _parse_refs(line, "@tabledef"):
                    if ref_def in table_refs:
                        _pulse_logger.warning(f"Redefinition of reference: {ref_def}")
                    table_refs[ref_def] = table_num
                    table_num += 1
            if "@figuredef" in line:
                for ref_def in _parse_refs(line, "@figuredef"):
                    if ref_def in fig_refs:
                        _pulse_logger.warning(f"Redefinition of reference: {ref_def}")
                    fig_refs[ref_def] = fig_num
                    fig_num += 1
            if "@equationdef" in line:
                for ref_def in _parse_refs(line, "@equationdef"):
                    if ref_def in eq_refs:
                        _pulse_logger.warning(f"Redefinition of reference: {ref_def}")
                    eq_refs[ref_def] = eq_num
                    eq_num += 1

        # Replace references
        def _replace_refs(line: str, tag: str, replacement: str, ref_defs: Dict[str, int]) -> str:
            if not line:
                return ""

            words = line.split()

            for idx, word in enumerate(words):
                if tag in word:
                    ref_name = get_table_tag(words[idx + 1])
                    if ref_name not in ref_defs:
                        _pulse_logger.fatal(f"Could not find {ref_name} in {ref_defs}")
                        exit(0)
                    words[idx] = word.replace(tag, f"{replacement}")
                    words[idx+1] = words[idx + 1].replace(ref_name, f"{ref_defs[ref_name]}")
            words[:] = [word for word in words if word]
            return " ".join(words)+'\n'

        for idx, line in enumerate(lines):
            if "@tabledef" in line:
                line = _replace_refs(line, "@tabledef", "Table", table_refs)
            if "@tableref" in line:
                line = _replace_refs(line, "@tableref", "Table", table_refs)
            if "@figuredef" in line:
                line = _replace_refs(line, "@figuredef", "Figure", fig_refs)
            if "@figureref" in line:
                line = _replace_refs(line, "@figureref", "Figure", fig_refs)
            if "@equationdef" in line:
                line = _replace_refs(line, "@equationdef", "Equation", eq_refs)
            if "@equationref" in line:
                line = _replace_refs(line, "@equationref", "Equation", eq_refs)
            lines[idx] = line


        return lines

    def _process_file(fpath: Path, ancestors: Set[Path]) -> List[str]:
        _pulse_logger.info(f"Processing file: {fpath}")
        # if "Hemorrhage_Validation.md" in str(fpath):
        #    print("Here")
        if fpath.resolve() in ancestors:
            raise RuntimeError(f"Circular insert involving {fpath} detected. Aborting.")
        ancestors.add(fpath.resolve())

        try:
            in_file = open(fpath, 'r')
            lines = in_file.readlines()
            in_file.close()
        except Exception as ex:
            _pulse_logger.error(f"Could not find {fpath.resolve()} {ex}")
            # TODO: What happens if you can't find a file
            return [f'<img src="./images/MissingTable.jpg"><center><i>Could not find "{fpath}"</i></center><br>\n']

        out_lines = []
        for line in lines:
            if "@insert" in line:
                stripped_line = line.strip()
                i_name = stripped_line[stripped_line.find(" "):].strip()
                f = Path(i_name)
                if not f.exists():
                    # Try to process this file assuming it is in the ref_dir
                    inserting = f.name
                    f = ref_dir.resolve() / inserting
                    if f.exists():
                        _pulse_logger.warning(f"Performing @insert, could not find {i_name}, going to try to insert {f}")
                        process_file(f, ref_dir, dest_dir, replace_refs=False, ancestors=ancestors.copy())
                        i_out_fname = dest_dir.resolve() / f.stem
                        i_out_fname = i_out_fname.with_suffix(i_out_fname.suffix+".md")
                        with open(i_out_fname, "r") as i_out:
                            i_lines = i_out.readlines()
                            out_lines.extend(i_lines)
                    else:
                        # If you get here, you are probably doing validation on a xlsx with multiple md files
                        # So that is fine, the validation pipeline should swing back again and properly insert files
                        _pulse_logger.warning(f"Could not find {i_name} for insertion, leaving insert in place")
                        out_lines.append(line)
                else:
                    _pulse_logger.info(f"Inserting {f}")
                    out_lines.extend(_process_file(f, ancestors.copy()))
            else:
                out_lines.append(line)
        return out_lines

    out_lines = _process_file(fpath, ancestors.copy())
    if replace_refs:
        out_lines = _process_references(out_lines)

    with open(out_fname, 'w') as out_file:
        out_file.writelines(out_lines)
    _pulse_logger.info("Writing processed file as: " + str(out_fname))


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    try:
        if len(sys.argv) == 3 or len(sys.argv) == 4:
            src_dir = Path(sys.argv[1])
            dest_dir = Path(sys.argv[2])
            if src_dir.is_dir():
                if len(sys.argv) == 4:
                    ref_dir = Path(sys.argv[3])
                else:
                    ref_dir = src_dir

                found = list(src_dir.rglob("*.[mM][dD]"))
                for f in found:
                    if f.is_dir():
                        continue # Not currently recursively processing directories
                    process_file(f, ref_dir, dest_dir, replace_refs=True)
            else:
                _pulse_logger.error(f"Cannot find source directory: {sys.argv[1]}")
        else:
            _pulse_logger.error(
                "Command arguments are: <Directory to process> "
                "<Directory to place processed files> [Directory where to find references for insert tags]"
        )
    except Exception as e:
        _pulse_logger.error(e)


if __name__ == "__main__":
    main()
