using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using ROBINinspect;

public class AppLogic : MonoBehaviour {

    public DiscoveryUI discoverScreen;
    public MonitorUI monitorScreen;

    // Use this for initialization
    void Start () {
        monitorScreen.gameObject.SetActive(false);
        discoverScreen.gameObject.SetActive(true);
    }

    // Update is called once per frame
    void Update () {
    }

    public void ShowMonitor(DiscoveryInfo info)
    {
        monitorScreen.Connect(info);
        discoverScreen.gameObject.SetActive(false);
        monitorScreen.gameObject.SetActive(true);
    }

    public void ShowDiscovery()
    {
        
        monitorScreen.Disconnect();
        discoverScreen.gameObject.SetActive(true);
        monitorScreen.gameObject.SetActive(false);
    }
}
