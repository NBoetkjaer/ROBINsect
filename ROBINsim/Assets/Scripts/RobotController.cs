using UnityEngine;
using System.Collections;

public class RobotController : MonoBehaviour {
    public static int NumLegs = 6;
    private Kinematic[] legs = new Kinematic[NumLegs];
    // Use this for initialization
    void Start () {
        string myName = this.name;
        for (int iLeg = 0; iLeg < NumLegs; ++iLeg) {
            GameObject gobj = GameObject.Find (myName + "/Leg" + (iLeg +1).ToString());
            if (gobj == null) continue;
            legs[iLeg] = gobj.GetComponent<Kinematic>();
        }
    }
    
    public Kinematic GetLeg(int legId)
    {
        if (legId < 0 || legId > NumLegs - 1)
            return null;
        return legs[legId];
    }
    // Update is called once per frame
    void Update ()
    {


    }

}
