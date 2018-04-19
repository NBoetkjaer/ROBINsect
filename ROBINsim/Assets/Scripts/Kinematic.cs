using UnityEngine;
using System;
using System.Collections;

public class Kinematic : MonoBehaviour {
    const float d1 = 5.9f;
    const float d2 = 8.7f;
    const float d3 = 8.7f;

    float[] jd = {0f, 0f, 0f};
    float[] ld = {d1, d2, d3};
    float[] la = {(float)Math.PI/2.0f,  0.0f, 0.0f};

    Vector3[] cgLinks = {new Vector3(d1/3.0f, 0.0f, 0.0f), // Center of gravity on link 1 in joint 1 coordinate system.
                         new Vector3(d2/3.0f, 0.0f, 0.0f),
                         new Vector3(d3/3.0f, 0.0f, 0.0f)};
    float[] massLinks = { 1.0f, 1.5f, 1.0f};

    private GameObject joint1;
    private GameObject joint2;
    private GameObject joint3;
    private GameObject tip;

    private float maxSpeed = 20f;
    private float smoothTime = 0.2f; 
    private Vector3 currentVelocity;
    private bool repeatTrajectory;
    private int pointIdx = -1; 
    private Vector3[] trajectory;


    // Use this for initialization
    void Awake ()
    {
        string myName = this.name;
        joint1 = GameObject.Find (myName + "/BaseLink/LinkEnd/Joint1");
        joint2 = GameObject.Find (myName + "/BaseLink/LinkEnd/Joint1/LinkEnd/Joint2");
        joint3 = GameObject.Find (myName + "/BaseLink/LinkEnd/Joint1/LinkEnd/Joint2/LinkEnd/Joint3");
        tip = GameObject.Find(myName + "/BaseLink/LinkEnd/Joint1/LinkEnd/Joint2/LinkEnd/Joint3/LinkEnd");
        //LineRenderer lineRenderer = gameObject.AddComponent<LineRenderer>();
    }
    
    // Update is called once per frame
    void Update () {

    }
    public float GetMass()
    {
        return massLinks [0] + massLinks [1] + massLinks [2];
    }
    public void RepeatTrajectory (bool repeat)
    {
        repeatTrajectory = repeat;
        if (repeatTrajectory && pointIdx == -1 && trajectory.Length >= 1) {
            pointIdx = 0;
        }
    }
    public void SetTrajectory(Vector3[] legTrajectory, bool repeatForever)
    {
        if (legTrajectory == null) {
            pointIdx = -1;
            return;
        }
        //LineRenderer lineRender = GetComponent<LineRenderer>();
        //lineRender.useWorldSpace = true;
        //lineRender.startWidth = 0.1f;
        //lineRender.endWidth = 0.1f;
        //lineRender.positionCount = legTrajectory.Length;
        //lineRender.SetPositions(legTrajectory);

        // Convert to local coordinates.
        trajectory = new Vector3[legTrajectory.Length];
        for(int i = 0; i < legTrajectory.Length; i++)
        {
            trajectory [i] = joint1.transform.InverseTransformPoint (legTrajectory [i]);
        }
        repeatTrajectory = repeatForever;
        pointIdx = 0;
    }
    public bool TrajectoryFinished()
    {
        return pointIdx == -1;
    }
    public void FollowTrajectory()
    {
        if (pointIdx < 0)
            return;
        
        int targetIdx = pointIdx + 1;
        if(targetIdx > trajectory.Length - 1) targetIdx = 0;

        Vector3 newGoal = Vector3.SmoothDamp(currentGoal, trajectory[targetIdx], ref currentVelocity, smoothTime, maxSpeed);
        if(SetGoal(newGoal))
            currentGoal = newGoal;

        if (Vector3.Distance (currentGoal, trajectory[targetIdx]) < 0.0001f) pointIdx++;
        if (pointIdx > trajectory.Length - 1)
        {
            if(repeatTrajectory) 
                pointIdx = 0;
            else
                pointIdx = -1;
        }


    }

    public void SetJoints(Vector3 angles_deg)
    {
        joint1.transform.localEulerAngles = new Vector3 (0f, 0f, 0f);
        joint1.transform.Rotate (0f, 0f, angles_deg.x);
        joint2.transform.localEulerAngles = new Vector3 (0f, 0f, 0f);
        joint2.transform.Rotate (0f, 0f, angles_deg.y);
        joint3.transform.localEulerAngles = new Vector3 (0f, 0f, 0f);
        joint3.transform.Rotate (0f, 0f, angles_deg.z);
        //currentGoal = calculateGoal (angles_deg);
    }
    public Vector3 GetJoints()
    {
        return new Vector3(
              joint1.transform.localEulerAngles.z
            , joint2.transform.localEulerAngles.z
            , joint3.transform.localEulerAngles.z);
    }
    private Vector3 currentGoal;
    public bool SetGoal(Vector3 pos)
    {
        // Solve the inverse kinematic for the 3 DOF leg.		
        float[] jointAngles = new float[3];
        // Joint 1 angle is given by:
        jointAngles[0] = Mathf.Atan2(pos.y, pos.x);
        // Find the end of link 1.
        Matrix4x4 jointLinkT = jointLinkTransform(0.0f, jointAngles[0], d1, (float)Math.PI/2.0f);
        Matrix4x4 invJointLinkT = jointLinkT.inverse;
        // Transform goal position into end of link 1 franejoint 1 frame.
        Vector3 tmpPos = invJointLinkT.MultiplyPoint3x4(pos);

        // The transformed goal point have a z value of zero.
        // Due to the geometry of the 3 DOF leg, the problem is now reduced to finding the intersection of two circles.
        // One is centered in (0,0) end of Link 1  the other centered around the transformed goal point.
        
        // http://paulbourke.net/geometry/circlesphere/
        float x, y, x2, y2, x3, y3;
        x = tmpPos.x;
        y = tmpPos.y;
        float dist = Mathf.Sqrt(x*x + y*y);
        if( dist > (d2 + d3) || dist < Mathf.Abs(d2 - d3) || dist == 0.0f)
        {
            Debug.Log("Goal un-reachable. " + pos.ToString());
            return false;
        }

        float a = ((d2*d2) - (d3*d3) + (dist*dist))/(2.0f*dist);
        if(d2 == a) 
        { 
            Debug.Log("Goal has only one solution.");
        }		
        float h = Mathf.Sqrt(d2*d2 - a*a);
        x2 = x * a / dist;
        y2 = y * a / dist;

        // Joint 2 angle is given by:
        if (false)
        {
            x3 = x2 + h * y / dist;
            y3 = y2 - h * x / dist;
        }
        else
        { //Or tbe other solution ...
            x3 = x2 - h * y / dist;
            y3 = y2 + h * x / dist;
        }
        jointAngles[1] = Mathf.Atan2(y3, x3);

        // Transform the goal point into the coordinate frame of link 2.
        jointLinkT = jointLinkTransform(0.0f, jointAngles[1], d2, 0.0f);
        invJointLinkT = jointLinkT.inverse;
        // Transform goal position into end of link 1 franejoint 1 frame.
        tmpPos = invJointLinkT.MultiplyPoint3x4(tmpPos);
        jointAngles[2] = Mathf.Atan2(tmpPos.y, tmpPos.x);

        jointAngles[0] *= 180.0f / (float)Math.PI;
        jointAngles[1] *= 180.0f / (float)Math.PI;
        jointAngles[2] *= 180.0f / (float)Math.PI;

        Vector3 angles = new Vector3(jointAngles[0], jointAngles[1], jointAngles[2]);
        SetJoints (angles);
        currentGoal = pos;
        return false;
    }
    public Vector3 GetGoal(Vector3 angles)
    {
        return currentGoal;
    }

    private Vector3 calculateGoal(Vector3 angles)
    {
        float[] ja = { angles.x * (float)Math.PI/180.0f,
                        angles.y * (float)Math.PI / 180.0f,
                        angles.z * (float)Math.PI/180.0f};
        
        Matrix4x4 transform = Matrix4x4.identity;
        for(int i = 0; i < 3; ++i)
        {
            Matrix4x4 jointLinkT = jointLinkTransform(jd[i], ja[i], ld[i], la[i]);
            transform = transform * jointLinkT;	
        }
        return new Vector3(transform[0,3], transform[1,3], transform[2,3]);
    }
    public Vector3 calculateCOG(Vector3 angles)
    {
        float[] ja = { angles.x * (float)Math.PI/180.0f,
            angles.y * (float)Math.PI / 180.0f,
            angles.z * (float)Math.PI/180.0f};
        Vector3 cog = Vector3.zero;
        Matrix4x4 transform = Matrix4x4.identity;
        for(int i = 0; i < 3; ++i)
        {
            // Joint  transform.
            transform = transform * jointTransform(jd[i], ja[i]);
            // Transform COG to base coordinate system.
            cog += transform.MultiplyPoint3x4 (cgLinks [i]) * massLinks[i];
            // Link transform.
            transform = transform * linkTransform(ld[i], la[i]);
        }
        // transform to parent coordinate system.
        return this.transform.TransformPoint(cog/GetMass());		

    }
    Matrix4x4 jointTransform(float jd, float ja)
    {
        float s = Mathf.Sin(ja);
        float c = Mathf.Cos(ja);
        Matrix4x4 jointT = Matrix4x4.identity;
        jointT[0,0] = c;
        jointT[1,1] = c;
        jointT[0,1] = -s;
        jointT[1,0] = s;
        jointT[2,3] = jd;
        return jointT;
    }
    Matrix4x4 linkTransform(float ld, float la)
    {
        float s = Mathf.Sin(la);
        float c = Mathf.Cos(la);
        Matrix4x4 linkT = Matrix4x4.identity;
        linkT[1,1] = c;
        linkT[2,2] = c;
        linkT[1,2] = -s;
        linkT[2,1] = s;
        linkT[0,3] = ld;
        return linkT;
    }
    Matrix4x4 jointLinkTransform(float jd, float ja, float ld, float la)
    {
        // Joint  transform.
        Matrix4x4 jointT = jointTransform(jd, ja);
        // Link transform.
        Matrix4x4 linkT = linkTransform(ld, la);	
        // Combined transform.
        return jointT * linkT;
    }

}
