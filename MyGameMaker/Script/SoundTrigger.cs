using HawkEngine;
public class SoundTrigger : MonoBehaviour
{
    private Collider soundCollider;
//     private AudioSource sound;

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

        //sound = gameObject.GetComponent<AudioSource>();

    }
    public override void Update(float deltaTime)
    {

    }
    public override void OnTriggerEnter(GameObject other)
    {
//         if(sound != null)
//         {
//             //sound.Play();
//         }
//         else
//         {
//             Engineson.print("Sound not found");
//         }
    }

    public override void OnTriggerExit(GameObject other)
    {
//         if (sound != null)
//         {
//             //sound.Stop();
//         }
//         else
//         {
//             Engineson.print("Sound not found");
//         }
    }

    public override void OnCollisionStay(GameObject other)
    {
        
    }
}