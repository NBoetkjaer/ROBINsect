using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using ROBINinspect;
public class DiscoveryUI : MonoBehaviour {
    public AppLogic app;
    public ScrollRect scrollView;
    public Button prefabRobin_info;
    private Discovery robinDiscovery = new Discovery();
    // Use this for initialization
    void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
        robinDiscovery.Update();
        FillScrollView();
    }

    public void SearchNetwork()
    {
        robinDiscovery.StartDiscovery();
    }

    public void FillScrollView()
    {
        int numChilds = scrollView.content.childCount;
        // Test if any changes.
        if (numChilds == robinDiscovery.dicoveryMap.Count)
            return;

        // Delete old entries.
        for (int i = 0; i < numChilds; ++i)
        {
            Transform trf = scrollView.content.GetChild(i);
            Button btn = trf.gameObject.GetComponent<Button>();
            if (btn != null)
            {
                btn.onClick.RemoveAllListeners();
            }
            Destroy(trf.gameObject);
        }
        // Insert new entries.
        foreach (DiscoveryInfo info in robinDiscovery.dicoveryMap.Values)
        {
            Button robinEntry = Instantiate(prefabRobin_info) as Button;
            if (robinEntry == null) return;

            robinEntry.transform.SetParent(scrollView.content.transform, false);
            robinEntry.name = info.ipAddr;
            robinEntry.onClick.AddListener(() => Connect(info));
            robinEntry.GetComponentInChildren<Text>().text = info.DisplayString();
            robinEntry.enabled = true;
        }
    }

    public void Connect(DiscoveryInfo info)
    {
        this.gameObject.SetActive(false);
        app.ShowMonitor(info);
    }
}
