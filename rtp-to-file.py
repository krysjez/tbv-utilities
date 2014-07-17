#!/usr/bin/python

# Point this at a directory, specify a FIXED number of ROIs and it will
# combine Turbo-BrainVoyager RTP (ROI timepoint) files into a single CSV!
# Utility script by Jessica Yang for ZI Mannheim

import string
import os

ans = "n"
while ans == "n":
        # Prompt for user input
        directory = raw_input("Full directory containing .rtp files: ")
        proj = raw_input("Project name (the part before -xx.rtp): ")
        n_roi = raw_input("Number of ROIs (must be unchanging across entire dataset!): ")
        n_files = raw_input("Total number of .rtp files: ")
        # Confirm
        print "Please confirm that your .rtp files are located in"
        print directory
        print "and the filenames look like " + proj + "-" + n_files + ".rtp, is this correct?"
        ans = raw_input("(y/n) ")

print "OK."
os.chdir(directory)
logfilename = raw_input("Name of file to save to: ")

# write header
log = open(logfilename, "w")
log.write("timestep, ")
for i in range(1, int(n_roi)+1): # headings for rois
        log.write("roi")
        log.write(str(i)+", ")
log.write("condition")
log.write(", trial")
log.write("\n")

# a dictionary keeps track of what trial each condition is on
cond_ctr = {}
prev_cond = None

for i in range(1, int(n_files)+1):
        filename = proj + "-" + str(i) + ".rtp"
        print "processing " + filename
        f = open(filename, "r")
        l = f.read()
        l = l.split() # create a list of n_roi, roi1, roi2, ... ,roiN, condition
        log.write(str(i)+", ")
        for j in range(1, int(n_roi)+1):
                log.write(l[j])
                log.write(", ")
        cond = l[-1] # condition is last entry in the .rtp
        cond_switch = False if (cond == prev_cond) else True
        log.write(cond)
        log.write(", ")
        if cond in cond_ctr:# If condition is in dictionary as a key,
                if cond_switch is True: # Is this a new condition?
                        cond_ctr[cond] += 1 # Then it's the next trial of that condition
        else:
                cond_ctr[cond] = 1 # trial 1 of that condition
        log.write(str(cond_ctr[cond]))
        log.write("\n")
        prev_cond = cond
log.close()
print "done!"
