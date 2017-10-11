#pragma once

#include "CommandLineModule.hpp"
//#include "../cmd.h"

void CommandLineModule::Publish()
{
}

void CommandLineModule::CreateNodes(BaseNode& rootNode)
{
}

void CommandLineModule::Notify()
{

}

void CommandLineModule::ProcessCmd(const char* pCmd, size_t dataLen)
{

    // Command line syntax: 
    // Set a value          $ cmdName=<value>
    // Response             $ <OK> or <FAIL>
    //
    // Query a value        $ ?cmdName
    // Response             $ <Value> or <FAIL>
    //
    // Set an attribute     $ cmdName attrName=<attrValue>
    // Response             $ <OK> or <FAIL>
    //
    // query an attribute   $ ?cmdName attrName
    // Response             $ <attrValue> or <FAIL>
    // 
    // query all info about a node:
    // Response             $ ??cmdName 
    //                      $ <FAIL> or multiple lines in the following format:
    //                      $ cmdName <nodeType>
    //                      $ attrName1 = <attrValue>
    //                      $ attrName2 = <attrValue>
    //                      $ ....
    //
    // Query all commands   $ --h, --help
    //
    // Add a new command    $ ++Add cmdName <absolute_node_path>
    //                      $ <OK> or <FAIL>
    // Remove a command     $ --delete cmdName
    //                      $ <OK> or <FAIL>
    // 
    // Ideas:
    // Continuously monitor a node:
    //                      $ ->cmdName [t=<timeout_ms>]
    // Response             $ <FAIL> or <Value> continuously sent at an interval of <timeout_ms> 
    //                      $ Sending a new command will end the monitoring state.


}
