/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/
 
//=====================================================================
///  @file example_system_top.h
 
///  @brief This class instantiates components that compose the TLM2 
///         example system called at_1_phase 

//=====================================================================
//  Original Authors:
//    Anna Keist, ESLX
//    Bill Bunton, ESLX
//    Jack Donovan, ESLX
//=====================================================================

#ifndef __EXAMPLE_SYSTEM_TOP_H__
#define __EXAMPLE_SYSTEM_TOP_H__

#include "reporting.h"                        // common reporting code
#include "at_target_1_phase.h"                // at memory target
#include "initiator_top.h"                    // processor abstraction initiator
#include "models/SimpleBusAT.h"               // Bus/Router Implementation

/// Top wrapper Module
class example_system_top             
: public sc_core::sc_module                   //  SC base class
{
public:
	
/// Constructor
	
  example_system_top              
  ( sc_core::sc_module_name name); 

//Member Variables  ===========================================================
  private:
  SimpleBusAT<2, 2>       m_bus;                  ///< simple bus
  at_target_1_phase       m_at_target_1_phase_1;  ///< instance 1 target
  at_target_1_phase       m_at_target_1_phase_2;  ///< instance 2 target
  initiator_top           m_initiator_1;          ///< instance 1 initiator
  initiator_top           m_initiator_2;          ///< instance 2 initiator
};

#endif /* __EXAMPLE_SYSTEM_TOP_H__ */
