using HawkEngine;
using System.Numerics;

public class AreaTrigger : MonoBehaviour, IInteractable
{
    private GameObject areaMessage;
    private Interaction areaImage;

    public bool hasInteracted { get; set; }

    public override void Awake()
    {
        base.Awake();
    }

    public override void Start()
    {
    }


    public override void Update(float deltaTime)
    {
       
    }
    public void Interact()
    {
        Engineson.print("Interacting with AreaTrigger");
        Engineson.Destroy(gameObject);
    }
}