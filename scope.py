import pyvisa # imports pyvisa 
import time # used for delay
import sys # error messages
from datetime import date

rm = pyvisa.ResourceManager()

# Find oscilloscope and open it, exiting if not found
for resource in rm.list_resources():
    if (resource.find("SGVJ0003827") != -1):
        scope = rm.open_resource(resource)

if (rm.list_opened_resources() == []):
    sys.exit("Oscilloscope not found. Did you plug it in?")

# Initial setup
scope.write("*RST") # reset to default settings
scope.write("HEAD ON") # turn on header for queries
scope.write("CH1:PROBEF:EXTA 1") # set attenutation

# Scaling/sampling settings
scope.write("HOR:MODE MAN") # set manual mode to adjust sample rate and scaling
scope.write("HOR:MODE:SAMPLER 2.5E6") # sample rate
scope.write("HOR:SCAL 4E-5") # horizontal scale
scope.write("HOR:POS 50") # center horizontal window
scope.write("CH1:SCAL 0.5") # set voltage scale to 0.5 V per division

# Trigger settings
scope.write("TRIG:A:EDGE:SLO EIT") # set trigger slope to either rising or falling
scope.write("TRIG:A:LEV:CH1 -500E-3") # set trigger to -160 mV ***CHANGE LATER***
scope.write("ACQ:STOPA SEQ") # stop after triggered (single/seq mode)


# Main loop
n = 0 # counter to avoid file duplication
while (True):

    scope.write("ACQ:STATE ON") # reset trigger

    # Wait for acquisition
    while ('1' in scope.query("ACQ:STATE?")):
        time.sleep(1)
   
    # Save file and increment counter
    scope.write(f"SAV:WAVE CH1, 'F:/ASHPB_test/test_{date.today()}_{n:03}.csv'")
    n += 1

    # Wait for data to save
    while ('1' in scope.query("BUSY?")):
        time.sleep(1)


# Scratch code for data transfer to computer
# scope.write("DAT:SOU CH1")
# scope.write("DAT:START 1")
# scope.write("DAT:STOP 10000")
# scope.write("WFMO:ENC BINARY")
# scope.write("WFMO:BYT_Nr 1")
# scope.write("HEAD 1")
# data = scope.query("WAVF?")

# Save on event settings (not needed)
# scope.write("ACTONEV:TRIG:ACTION:SAVEWAVE:STATE ON") # set scope to save waveform on trigger
# scope.write("SAVEONEV:FILED 'F:/ASHPB_test'") # set file destination
# scope.write("SAVEONEV:FILEN 'test_save'") # set file name
# scope.write("SAVEONEV:FILEF SPREADS") # set file format (spreadsheet is .csv)
# scope.write("ACTONEV:EN 1") # enable act on event

