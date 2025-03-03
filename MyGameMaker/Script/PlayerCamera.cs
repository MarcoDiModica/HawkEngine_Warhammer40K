using System.Numerics;
using HawkEngine;

public class PlayerCamera : MonoBehaviour
{
    private GameObject playerRef;
    private Camera cameraRef;
    private PlayerInput playerInput;

    public float smoothness = 9.0f;

    public override void Start()
    {
        playerRef = GameObject.Find("Player");
        cameraRef = gameObject.GetComponent<Camera>();

        if (playerRef == null)
        {
            Engineson.print("ERROR: PlayerCamera requires a GameObject named 'Player' in the scene!");
            return;
        }

        if (cameraRef == null)
        {
            Engineson.print("ERROR: PlayerCamera requires a Camera component!");
            return;
        }

        cameraRef.SetFollowTarget(playerRef, new Vector3(0, 20, -10), 0, true, true, true, smoothness);
    }

    public override void Update(float deltaTime)
    {
        if (Input.GetLeftStick() != Vector2.Zero)
        {
            cameraRef?.SetDistance(-4);
        }
        else if (Input.GetRightStick() != Vector2.Zero)
        {
            cameraRef?.SetDistance(-4);
        }
        else
        {
           cameraRef?.SetDistance(0);
        }
    }
}