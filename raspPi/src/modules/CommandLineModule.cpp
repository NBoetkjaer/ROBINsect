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
    // Add a new command. (Creates a mirror node to <absolute_node_path> under the command node.) 
	//						$ ++Add cmdName <absolute_node_path>
    //                      $ <OK> or <FAIL>
	//
    // Remove a command. Deletes a mirror node under the command node.
	//						$ --delete cmdName
    //                      $ <OK> or <FAIL>
    // 
    // Ideas:
    // Continuously monitor a node:
    //                      $ ->cmdName [t=<timeout_ms>]
    // Response             $ <FAIL> or <Value> continuously sent at an interval of <timeout_ms> 
    //                      $ Sending a new command will end the monitoring state.
	//
	// Note: Create/Destroy nodes migth could make add/delete command obsolete?
	// Create a new node in the node tree. <nodeType> is the type of the end node. All other nodes are assumed to be of type 'node'.
	//                      $ ++Create nodeName type=<nodeType> path=<absolute_node_path>
    // Response             $ <OK> or <FAIL>
	//
	// Delete a node from the node tree. (node must be an end node/no children) 
    // What if some one is depending on node ?? probably not a good idea to delete nodes?
	//						$ --Destroy <absolute_node_path>



}
