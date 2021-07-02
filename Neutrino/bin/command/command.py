# Neutrino Operating System Project
# Command shell interpreter
# Date: 26/05/2021

include neutrino

# Global variables

!('link neutrino.lnx')
path = "0:\"

# Commands

def command_ver():
    print(@NtrGetInformationString(STRING_ABOUT_OS))

command_ver()