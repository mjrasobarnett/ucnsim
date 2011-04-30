#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <iostream>
#include <sstream>
#include <sys/stat.h> // Required by Progress bar
#include <iomanip>
#include <string>

using namespace std;

namespace ProgressBar {
   
   //_____________________________________________________________________________
   static void PrintProgress(int entry, float nEntriesF, int mintime)
   {
    // Written by Nicholas Devenish - 01/2010
     int nEntries = (int)nEntriesF;

     // Are we streaming to a file? If so, show the old style
     struct stat buf;
     fstat(fileno(stdout), &buf);
     // Check if we are a file, or a pipe (i.e. in case the user tees)
     const bool isFile = buf.st_mode & (S_IFREG | S_IFIFO) ;
     if (isFile) {
       float fract = ceil(nEntries/20.);  
       if (ceil(((float)entry)/fract)==((float)entry)/fract)
       {
           cerr 
             <<"Fraction of loop complete: "<<entry 
             <<"/"<<nEntries<<"  ("
             <<(int)(100.*entry/nEntries)<<"%)"<<endl;
       }
       return;
     }


     // How wide should we make the progress bar?
     const int width = 70;
     // How long is the string for entries?
     static int countlen = -1;
     // How long is our progress bar?
     static int barlen = -1;
     // The entry number of the next bar entry
     static int nextbar = -1;
     // When did we start?
     static time_t starttime = 0;
     // when do we next update?
     static time_t nextupdate = 0;
     // If we are processing the first entry, reset everything
     if (entry <= 1)
     {
       // Get the new length of the entry string
       countlen = (int)ceil(log10(nEntries)) + 1;
       nextbar = -1;
       starttime = time(NULL);

       barlen = width - 14 - countlen*2 - 1;

       // Don't update until we get to the minimum time
       nextupdate = starttime + mintime;
     }

     // Check here to see if we should update; otherwise, return
     // Check to see if the bar would update
     // or, alternatively, it is time to refresh.
     if ((time(NULL) < nextupdate) && (entry < nextbar || (nextbar == -1)))return;
     nextupdate = time(NULL) + 10;

     // Because this is used in several places, make it here
     float frac = (float)entry / (float)nEntries;

     // Prepare the progress bar string
     string bar;
     if (entry <= nEntries)
     {
       // Work out how many characters we are in
       int numeq = floor(frac*barlen);

       // Work out when the next bar will occur
       nextbar = (int)((float)(numeq+1)/(float)barlen*nEntries);
       //cerr << "Next bar at: " << nextbar << "        " << endl;
       bar = string(numeq,'=');
       bar += string(barlen - numeq, ' ');
     } else if (entry > nEntries) {
       // We have gone over. Oh no!
       bar = string(barlen, '+');
     } else if (entry < 0) {
       // Somehow, we are below zero. Handle it nonetheless
       bar = string(barlen, '-');
     }


     // Prepare the ETA
     float elapsed_time = (float)(time(NULL) - starttime);
     float time_left = -60;
     if (frac > 1e-6) {
       time_left = (elapsed_time / frac) - elapsed_time;
     }
     int mins, seconds;
     mins    = (int)floor(time_left / 60.0f);
     seconds = (int)floor(time_left - (float)(mins*60.0f));
     // ETA;
     std::ostringstream ETA;

     ETA << "ETA ";
     if ((mins < 0 || seconds < 0) || (mins == 0 && seconds == 0)) {
       ETA << "--:--";
     } else {
       ETA << std::setfill('0') << std::setw(2) << mins << ":" << std::setw(2) << seconds;
     }

     cerr << " Progress: [" << bar << "] "
          << std::setw(countlen) << entry << "/" << nEntries
          << " " << ETA.str()
          << '\r'
          << std::flush;
     // Move to the next line, if this is the final entry!
     if (entry == nEntries) {
       cerr << endl;
     }
   }
   
}

#endif /*PROGRESS_BAR_H*/
