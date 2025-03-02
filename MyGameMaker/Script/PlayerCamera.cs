using System.Numerics;
using HawkEngine;

public class PlayerCamera : MonoBehaviour
{
    private GameObject playerRef;
    private Camera cameraRef;
    private PlayerInput playerInput;

    public float smoothness = 0.99f;

    public override void Start()
    {
        playerRef = GameObject.Find("Player");
        cameraRef = gameObject.GetComponent<Camera>();
        playerInput = playerRef?.GetComponent<PlayerInput>();

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

        if (playerInput == null)
        {
            Engineson.print("ERROR: PlayerCamera requires a PlayerInput component on the player!");
            return;
        }

        cameraRef.SetFollowTarget(playerRef, new Vector3(0, 15, -10), -5, true, true, true, smoothness);
    }

    public override void Update(float deltaTime)
    {

    }

    public void LookDirection(Vector3 lookDirection)
    {
    }
}