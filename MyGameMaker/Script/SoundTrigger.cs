using HawkEngine;
public class SoundTrigger : MonoBehaviour
{
    private Collider soundCollider;
    private Audio sound;

    public override void Awake()
    {
        base.Awake();
    }

    public override void Start()
    {
        soundCollider = gameObject.GetComponent<Collider>();
        if (soundCollider == null)
        {
            Engineson.print("Error");
        }

        sound = gameObject.GetComponent<Audio>();

    }
    public override void Update(float deltaTime)
    {

    }
    public override void OnTriggerEnter(GameObject other)
    {
        if(sound != null)
        {
            sound.Play();
        }
        else
        {
            Engineson.print("Sound not found");
        }
    }

    public override void OnCollisionStay(GameObject other)
    {
        
    }
}