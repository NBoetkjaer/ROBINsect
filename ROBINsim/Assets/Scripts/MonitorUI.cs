using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using ROBINinspect;

class LegJoints
{
    public static int numJoints = 3;
    private int legIdx = -1;    
    private FloatNode[] jointAnglesNodes;
    public bool LookupNodes(BaseNode rootNode, int legID)
    {
        legIdx = -1;
        FloatNode[] nodes = new FloatNode[numJoints];
        for (int jointID = 0; jointID < numJoints; jointID++)
        {
            string path = string.Format("/Insect/Legs/l{0}/Joints/j{1}/jointAngle", legID, jointID);
            nodes[jointID] = rootNode.FindNode(path) as FloatNode;
            if (nodes[jointID] == null)
                return false;
        }        
        // Every thing went ok - update variables.
        jointAnglesNodes = nodes;
        legIdx = legID;
        return true;
    }

    public FloatNode JointNode(int jointID)
    {
        if (jointID < 0 || jointID > (numJoints - 1))
            return null;
        return jointAnglesNodes[jointID];
    }

    public void UpdateLeg(RobotController robot)
    {
        Kinematic leg = robot.GetLeg(legIdx);
        if (leg == null) return;
        Vector3 jointsVec = new Vector3();
        for (int jointID = 0; jointID < numJoints; jointID++)
        {
            jointsVec[jointID] = jointAnglesNodes[jointID].Value;
        }
        leg.SetJoints(jointsVec);
    }
}
public class MonitorUI : MonoBehaviour
{
    public AppLogic app;
    public RobotController robotControl;
    private ROBINinspect.Monitor monitor = new Monitor();
    private bool connected = false;
    private BoolNode testTrajectoryNode;

    private LegJoints[] legJointAngles;
    // Use this for initialization
    void Start()
    {
        monitor.TransactionRecieved += Monitor_TransactionRecieved;
    }

    public void Connect(DiscoveryInfo info)
    {
        monitor.info = info;
        monitor.Connect();
    }
    public void Disconnect()
    {
        testTrajectoryNode = null;
        legJointAngles = null;
        connected = false;
        monitor.Close();
        app.ShowDiscovery();
    }

    // Update is called once per frame
    void Update()
    {
        monitor.Update();
        if (!monitor.IsConnected)
        {
            legJointAngles = null;
            if(connected)
            {
                Disconnect();
            }
        }
        else if (legJointAngles == null && monitor.rootNode != null)
        {
            connected = true;
            testTrajectoryNode = monitor.rootNode.FindNode("/Insect/TestTrajectory") as BoolNode;
            // Initialize leg joint nodes.
            legJointAngles = new LegJoints[RobotController.NumLegs];
            bool ok = true;
            for (int legId = 0; legId < RobotController.NumLegs; ++legId)
            {
                legJointAngles[legId] = new LegJoints();
                ok &= legJointAngles[legId].LookupNodes(monitor.rootNode, legId);
            }
            if (!ok)
            {
                // Error ?
                Disconnect();
            }
        }
    }

    void UpdateJoints()
    {
        if (legJointAngles == null)
            return;
        for (int legId = 0; legId < RobotController.NumLegs; ++legId)
        {
            if (legJointAngles[legId] == null)
                return;
            legJointAngles[legId].UpdateLeg(robotControl);
        }
    }

    public void ToggleTestTrajectory()
    {
        if(testTrajectoryNode != null)
        {
            GameObject tglTestTrajectory = GameObject.Find("ToggleTestTrajectory");
            UnityEngine.UI.Toggle tg = tglTestTrajectory.GetComponent<UnityEngine.UI.Toggle>();

            monitor.rootNode.ClearAllChanges();
            testTrajectoryNode.Value = tg.isOn;
            monitor.SendTreeUpdates();
        }
    }
    private void Monitor_TransactionRecieved(object sender, System.EventArgs e)
    {
        UpdateJoints();
        monitor.rootNode.ClearAllChanges();
    }
}
