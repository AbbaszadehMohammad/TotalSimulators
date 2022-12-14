/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the router
 */

#ifndef __NOXIMROUTER_H__
#define __NOXIMROUTER_H__

#include <systemc.h>
#include "NoximMain.h"
#include "NoximBuffer.h"
#include "NoximStats.h"
#include "NoximGlobalRoutingTable.h"
#include "NoximLocalRoutingTable.h"
#include "NoximReservationTable.h"
using namespace std;

extern unsigned int drained_volume;

SC_MODULE(NoximRouter)
{

    // I/O Ports
    sc_in_clk clock;		                  				// The input clock for the router
    sc_in <bool> reset;                           			// The reset signal for the router

    sc_in  <NoximFlit> flit_rx      [DIRECTIONS + 2];	  	// The input channels (including local one)
    sc_in  <bool     > req_rx       [DIRECTIONS + 2];	  	// The requests associated with the input channels
    sc_out <bool     > ack_rx       [DIRECTIONS + 2];	  	// The outgoing ack signals associated with the input channels
                                    
    sc_out <NoximFlit> flit_tx      [DIRECTIONS + 2];   	// The output channels (including local one)
    sc_out <bool     > req_tx       [DIRECTIONS + 2];	  	// The requests associated with the output channels
    sc_in  <bool     > ack_tx       [DIRECTIONS + 2];	  	// The outgoing ack signals associated with the output channels

    sc_out <int> free_slots         [DIRECTIONS + 1];
    sc_in  <int> free_slots_neighbor[DIRECTIONS + 1];
	             
	sc_out <bool>on_off             [4];				    // Information to neighbor router that if this router be throttled
	sc_in  <bool>on_off_neighbor    [4];	                // Information of throttling that if neighbor router be throttled
	
	/******* Ports for the signal transmission of Predictive Tempeature (PDT) ********/
	sc_out <float> PDT              [DIRECTIONS];           //Jimmy added on 2014.12.16
	sc_in <float>  PDT_neighbor     [DIRECTIONS];           //Jimmy added on 2014.12.16
	/********************************************************************************/
	
	/******* Ports for the signal transmission of current Buffer Depth********/
	//buf[A][B]: The output port in A side, which delivers the input buffer length
	//           in the current router's B side.
	sc_out <float> buf              [DIRECTIONS+2][DIRECTIONS];
	sc_in <float>  buf_neighbor     [DIRECTIONS+2][DIRECTIONS];
	/********************************************************************************/
	
	sc_out<int>       RCA_data_out[8]; //Jimmy added on 2012.06.27
	sc_in<int>        RCA_data_in[8]; //Jimmy added on 2012.06.27
	
	sc_out<double>	 monitor_out[DIRECTIONS];
	sc_in<double>	 monitor_in [DIRECTIONS];
	
	// Neighbor-on-Path related I/O
    sc_out < NoximNoP_data > NoP_data_out[DIRECTIONS];
    sc_in  < NoximNoP_data > NoP_data_in [DIRECTIONS];
	//
	sc_out < NoximNoP_data > vertical_free_slot_out;
    sc_in  < NoximNoP_data > *vertical_free_slot_in;
    // Registers
	unsigned int          _man_set_max_buffer_size ;
    int                    local_id                ;     // Unique ID
    int                    routing_type            ;     // Type of routing algorithm
    int                    selection_type          ;
    NoximBuffer            buffer[DIRECTIONS + 2]  ;	 // Buffer for each input channel 
    NoximStats             stats                   ;	 // Statistics
    NoximLocalRoutingTable routing_table           ;	 // Routing table
    NoximReservationTable  reservation_table       ;	 // Switch reservation table
    int                    start_from_port         ;	 // Port from which to start the reservation cycle
	int                    buffer_allocation_time  ;   //Calvin added 
    
	int		               cnt_neighbor            ;	 // counter for packets from neighbor routers		
	int		               cnt_received            ;
	double                 buffer_util             ;
	double                 buffer_used             ;
	unsigned int           local_drained           ;
	/*Jack add on 2019.09.23*/
	double eva_o;
	double tar_o;
	int in_neu;
	int o_neu;
	//double reward;
	double lr;
	double gamma;
	int D_count;
	int node_no;
	double eva_L1_w[3][6];
	double in[5];
	double tar_L1_w[3][6];
	void ReadWeight();
	double eva_L1[3];
	double tar_L1[3];
	double BTD;
	int test;
	/**/
	int tran_permit;                                  //use to slow down the router when it is potencial overheat (Jimmy modified on 2011.11.15)
    // Functions  
    unsigned long          getRoutedFlits        ();     // Returns the number of routed flits 
	unsigned long          getRoutedFlits   (int i);     // Returns the number of routed flits
    unsigned long          getRoutedDWFlits      ();     // Returns the number of routed flits 	
	unsigned long          getWaitingTime   (int i);
	unsigned long          getTotalWaitingTime   ();
	unsigned long          getRoutedPackets      ();
    unsigned long          getFlitsCount         (){ return _buffer_pkt_count; };     // Returns the number of flits into the router
	unsigned long          getMsgDelay           (){ return _buffer_pkt_msg_delay; };
	unsigned long          getNiDelay            (){ return _buffer_pkt_ni_delay;  };
	unsigned long          getNwDelay            (){ return _buffer_pkt_nw_delay;  };
	void                   CalcDelay             ( vector <int > & );
	int                    getFlitRoute     (int i);
	int                    getDirAvailable  (int i);
    double                 getPower              ();     // Returns the total power dissipated by the router
	void                   TraffThrottlingProcess();
	void                   IntoEmergency         ();
	void                   IntoSemiEmergency(int emergency_level); //Jimmy added on 2012.04.12
	void                   OutOfEmergency        ();
	void                   IntoCleanStage        (); //Jimmy added on 2017/05/02 - Reset the buffer length in clean stage
	void                   OutOfCleanStage       (); //Jimmy added on 2017/05/02 - Recover the adjusted buffer length after clean stage
	void configure(const int _id, const double _warm_up_time,
		   const unsigned int _max_buffer_size,
		   NoximGlobalRoutingTable & grt);
	void TBDB(); //Thermal-aware Dynamic Buffer Allocation (Jimmy modified on 2014.12.22)
    // Constructor

    SC_CTOR(NoximRouter) {
	vertical_free_slot_in = new sc_in  < NoximNoP_data >[NoximGlobalParams::mesh_dim_z];
		for(int i=0;i<5;i++)
			in[i] = -1;
	SC_METHOD(rxProcess);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(txProcess);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(bufferMonitor);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(RCA_Aggregation);
	sensitive << reset;
	sensitive << clock.pos();

    SC_METHOD(TraffThrottlingProcess);
    sensitive << reset;
    sensitive << clock.pos();
	
	//SC_METHOD(TBDB); //Trigger the TBDB block (Jimmy added on 2014.12.22)
	//sensitive << reset;
	//sensitive << clock.pos();

    }

  private:
	//unsigned int _man_set_max_buffer_size;
	void                   rxProcess             ();     // The receiving process
    void                   txProcess             ();     // The transmitting process
	void                   trans_fun             ();     //the main function of transmission (Jimmy added on 2012.01.19)
    void                   bufferMonitor         ();
	
    // performs actual routing + selection
    int route(const NoximRouteData & route_data);
	
    // wrappers
    int           selectionFunction  (const vector <int> &directions, const NoximRouteData & route_data);
    vector < int >routingFunction    (const NoximRouteData & route_data);
	vector < int >DW_layerSelFunction(const int select_routing, const NoximCoord& current, const NoximCoord& destination, const NoximCoord& source, int dw_layer);

    // selection strategies
    int selectionRandom     (const vector <int> & directions                                   );
    int selectionBufferLevel(const vector <int> & directions                                   );
    int selectionNoP        (const vector <int> & directions, const NoximRouteData & route_data);
	int selectionProposed   (const vector <int> & directions, const NoximRouteData & route_data);
    int selectionRCA        (const vector<int>& directions, const NoximRouteData& route_data); //RCA function (Jimmy added 2012.06.27)
	int selectionThermal     (const vector <int> & directions, const NoximRouteData & route_data); //selection function for PTDBA based proactive routing (Jimmy added on 2014.12.17)
	
	// routing functions
	vector < int >routingXYZ             (const NoximCoord & current                          ,const NoximCoord & destination);
	vector < int >routingZXY             (const NoximCoord & current                          ,const NoximCoord & destination);
	vector < int >routingWestFirst       (const NoximCoord & current                          ,const NoximCoord & destination);
    vector < int >routingNorthLast       (const NoximCoord & current                          ,const NoximCoord & destination);
    vector < int >routingNegativeFirst   (const NoximCoord & current                          ,const NoximCoord & destination);
	vector < int >routingLookAhead       (const NoximCoord & current                          ,const NoximCoord & destination);
    vector < int >routingFullyAdaptive   (const NoximCoord & current                          ,const NoximCoord & destination);
    vector < int >routingOddEven         (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination);
    vector < int >routingDyAD            (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination);
	vector < int >routingOddEven_Z       (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination);
	vector < int >routingOddEven_for_3D  (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination);
	vector < int >routingOddEven_3D      (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination);
	vector < int >routingDownward        (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination);
	vector < int >routingOddEven_Downward(const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination, const NoximRouteData& route_data); //Foster modified
    vector < int >routingTableBased      (const NoximCoord & current,const int dir_in         ,const NoximCoord & destination);
	vector < int >routingDLADR           (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination,const int select_routing, int dw_layer);  
	vector < int >routingDLAR            (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination,const int select_routing, int dw_layer);  
	vector < int >routingDLDR            (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination,const int select_routing, int dw_layer);  
	vector < int >routingTLAR_DW         (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination); 
	vector < int >routingTLAR_DW_VBDR    (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination); 
	vector < int >routingTLAR_DW_IPD     (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination); 
	vector < int >routingTLAR_DW_ADWL    (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination); 
	vector < int >routingTLAR_DW_ODWL    (const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination, int dw_layer); 
	vector < int >routingTLAR_DW_ODWL_IPD(const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination, int dw_layer); 
	
	unsigned long routed_flits[DIRECTIONS + 2]               ;
	unsigned long waiting     [DIRECTIONS + 2]               ;
	unsigned long routed_DWflits                             ;
	unsigned long routed_packets                             ;
	
	
	
	
    void          RCA_Aggregation                    ()      ;
	bool          inCongestion                       ()      ;
	NoximNoP_data getCurrentNoPData                  () const;
    void          NoP_report                         () const;
    int           reflexDirection       (int direction) const;
    int           getNeighborId(int _id, int direction) const;
	bool          Adaptive_ok(NoximCoord &sour,NoximCoord &dest);
    int           NoPScore(const NoximNoP_data & nop_data, const vector <int> & nop_channels, int candidate_id) const; //Jimmy added candidatw_id on 2012.06.27
	//Run-time Thermal Management
	
	bool 	               _emergency               ;     // emergency mode
	int 	               _emergency_level         ;	 // emergency level
	bool				   _emergency_trigger_flag	;	//have been trigger or not.
	bool	               _throttle_neighbor       ;	 // throttle port from neighbor routers
	//unsigned int          _man_set_max_buffer_size  ;    //Calvin added 
	unsigned long          _total_waiting;
	unsigned long          _buffer_pkt_count        ;
	unsigned long          _buffer_pkt_msg_delay    ;
	unsigned long          _buffer_pkt_ni_delay     ;
	unsigned long          _buffer_pkt_nw_delay     ;
	int                   _reset_buf[DIRECTIONS]    ; //Jimmy added on 2017.05.02

	//int throt[8][8];
	ifstream win;
	/**/

};

#endif
