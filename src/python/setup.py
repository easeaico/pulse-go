# Distributed under the Apache License, Version 2.0.
#   See accompanying NOTICE file for details.

from setuptools import setup, find_packages

with open('../../ReadMe.md') as readme:
    long_description = readme.read()

setup(
    name='pulse',
    version='2.3.0',
    description='A Physiology Simulator Engine',
    long_description=long_description,
    long_description_content_type='text/MarkDown',
    url='https://pulse.kitware.com/',
    maintainer='Kitware, Inc.',
    maintainer_email='kitware@kitware.com',
    include_package_data=True,
    # find_package descends into all subdirectories
    packages=find_packages(),
    install_requires=[
        'six',
        'numpy',
        'matplotlib',
        'pandas',
        'openpyxl',
        'pycel'
    ],
)
