using HawkEngine;
using System.Numerics;

public class AreaTrigger : MonoBehaviour, IInteractable
{
    private GameObject areaMessage;
    private UIImage areaImage;

    public bool hasInteracted { get; set; }

    public override void Awake()
    {
        base.Awake();
    }

    public override void Start()
    {
        areaMessage = GameObject.Find("dialogueText"); 
        if (areaMessage != null)
        {
            areaImage = areaMessage.GetComponent<UIImage>();
            if (areaImage != null)
            {
                areaImage.SetImageEnabled(false);
            }
            else
            {
                Engineson.print("ERROR: AreaText does not have a UIImage component.");
            }
        }
        else
        {
            Engineson.print("ERROR: Missing AreaText UI element.");
        }
    }


    public override void Update(float deltaTime)
    {
       
    }
    public void SetTextVisibility(bool visible)
    {
        if (areaImage != null)
        {
            areaImage.SetImageEnabled(visible);
        }
    }

    public void Interact()
    {
        Engineson.print("Interacting with AreaTrigger");
        Engineson.Destroy(gameObject);
    }
}