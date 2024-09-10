# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import sys
import logging
import argparse
from pathlib import Path

_pulse_logger = logging.getLogger('pulse')

if __name__ == "__main__":

    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    logging.getLogger("pycel").setLevel(logging.WARNING)

    reports = list()
    reports.append(Path("./test_results/CDMUnitTests.html"))
    reports.append(Path("./test_results/EngineUnitTests.html"))
    reports.append(Path("./test_results/ScenarioVerification.html"))
    reports.append(Path("./test_results/DrugPKVerification.html"))
    reports.append(Path("./test_results/PatientVerification.html"))
    reports.append(Path("./test_results/PatientValidation.html"))

    html_file = "./test_results/PulseTestReport.html"
    _pulse_logger.info(f"Writing {html_file}")
    f = open(html_file, "w")
    f.writelines("<html>\n")
    f.write("<body>\n")
    for report in reports:
        f.writelines("<br>\n")
        with open(report) as file:
            while line := file.readline():
                line = line.replace("<html>", "")
                line = line.replace("<body>", "")
                line = line.replace("</body>", "")
                line = line.replace("</html>", "")
                f.write(line+"\n")
    f.writelines("</body>\n")
    f.write("</html>\n")