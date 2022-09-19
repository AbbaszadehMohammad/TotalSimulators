/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the switch reservation table
 *
 *
 * Edited by Amin Norollah @BALRUG (2017 Feb 1)
 * 
 */

#ifndef __NOXIMRESERVATIONTABLE_H__
#define __NOXIMRESERVATIONTABLE_H__

#include <cassert>
#include "NoximMain.h"
using namespace std;

class NoximReservationTable {
  public:

    NoximReservationTable();

    // Clear reservation table
    void clear();

    // check if port_out is reservable
    bool isAvailable(const int port_out, const int vc_out) const;

    // Connects port_in with port_out. Asserts if port_out is reserved
    void reserve(const int port_in, const int vc_in, const int port_out, const int vc_out);

    // Releases port_out connection. 
    // Asserts if port_out is not reserved or not valid
    void release(const int port_out, const int vc_out);

    // Returns the output port connected to port_in.
    int getOutputPort(const int port_in, const int vc) const;
	int getOutputVc(const int port_in, const int vc) const;

    // Makes output port no longer available for reservation/release
    void invalidate(const int port_out);

  private:

	  int rtable1	[DIRECTIONS + 2][DEFAULT_NUM_VC	]	;
	  int rtable2	[DIRECTIONS + 2][DEFAULT_NUM_VC	]	;

    // vector < vector < int > >rtable;	// reservation vector: rtable[i] gives the input
	// vector < int >vc_in;
    // port whose output port 'i' is connected to
};

#endif
