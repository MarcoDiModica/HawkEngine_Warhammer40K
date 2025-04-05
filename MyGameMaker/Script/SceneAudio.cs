using HawkEngine;
public class SceneAudio : MonoBehaviour
{

    private Audio sound;

    public override void Awake()
    {
        base.Awake();
    }

    public override void Start()
    {
        sound = gameObject.GetComponent<Audio>();
        if (sound == null)
        {
            Engineson.print("ERROR: Audio not found");
        }
        else
        {
            sound.Play();
        }

    }
    public override void Update(float deltaTime)
    {

    }
 
}