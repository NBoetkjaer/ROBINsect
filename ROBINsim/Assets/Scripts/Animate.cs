using UnityEngine;
using System.Collections;

public class Animate : MonoBehaviour {
    public GameObject cogObj;
    private Vector3 cogBody = new Vector3 (0f, 0f, 0f);
    private const float bodyMass = 4.0f;
    private Vector3 cog;
    private Vector3 gravity = new Vector3(0f, -9.8f, 0f);
    private const int numLegs = 6;
    private Kinematic[] legs = new Kinematic[numLegs];
    private bool[] supportingLegs = new bool[numLegs];

    // Use this for initialization
    void Start () {
        string myName = this.name;
        for (int iLeg = 0; iLeg < numLegs; ++iLeg) {
            GameObject gobj = GameObject.Find (myName + "/Leg" + (iLeg +1).ToString());
            legs[iLeg] = gobj.GetComponent<Kinematic>();
            //legs [iLeg].SetGoal (new Vector3(0.3f, 0.0f, -0.1f));
        }
            
        GenerateTestTrajectory ();
    }
        
    void GenerateTestTrajectory()
    {
        Vector3[] trajectory = new Vector3[2];
        for (int i = 0; i < legs.Length; i++) {

            //float dir = Mathf.Sign(legs [i].transform.localPosition.z);
            float dir = 0 == (i & 1) ? -1f : 1f;
            trajectory[0] = trajectory[1] = legs[i].transform.position;
            trajectory[0] += new Vector3(dir * 20f, -3f, 5f);
            trajectory[1] += new Vector3(dir * 20f, -3f, -5f);
            //trajectory[1] = new Vector3(15f, -3f, legs[i].transform.localPosition.z - 2f);

            //trajectory [0] = new Vector3 (legs [i].transform.position.x * 2f - dir* 0.00f, 0f, 1.7f*legs [i].transform.position.z);
            //trajectory [1] = new Vector3 (legs [i].transform.position.x * 2f + dir* 0.07f, 0f, 1.7f*legs [i].transform.position.z);
            legs[i].SetTrajectory (trajectory, false);
        }
    }

    // Update is called once per frame
    void Update () {

        bool start = true;
        cog = Vector3.zero;
        float legMass = 0f;
        for (int iLeg = 0; iLeg < numLegs; ++iLeg) {
            // Follow trajectory.
            legs[iLeg].FollowTrajectory ();

            // Calculate COG (super position)
            Vector3 angles = legs[iLeg].GetJoints ();
            float mass = legs[iLeg].GetMass ();
            cog += legs [iLeg].calculateCOG (angles) * mass;
            legMass += mass;

            // Synchronize movements.
            start &= legs [iLeg].TrajectoryFinished ();
        }
            
        cog += transform.TransformPoint(cogBody) * bodyMass;
        cog /= (legMass + bodyMass);
        cog.y = 0.0f;
        if (cogObj != null) {
            cogObj.transform.position = cog;
        }

        // Start synchronized movement.
        if (start) {
            foreach (Kinematic leg in legs) {
                leg.RepeatTrajectory (true);
            }
        }
    }


    private Vector3 CheckStability()
    {
        // Project COG to ground plane using the gravity vector.

        // Determine minimum distance from projected point to vertexes of the convex hull
        // that is spanned by the legs that touches the ground.

        // Return the vector from projected COG to the nearets side of 

        // Simplified approach. 
        // projection.

        return Vector3.zero;
    }
        
}
