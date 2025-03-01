using System.Numerics;
using HawkEngine;

public class PlayerController : MonoBehaviour
{
    private PlayerInput playerInput;
    private PlayerMovement playerMovement;
    private PlayerDash playerDash;
    private PlayerShooting playerShooting;

    public override void Start()
    {
        playerInput = gameObject.GetComponent<PlayerInput>();
        playerMovement = gameObject.GetComponent<PlayerMovement>();
        playerDash = gameObject.GetComponent<PlayerDash>();
        playerShooting = gameObject.GetComponent<PlayerShooting>();

        if (playerInput == null || playerMovement == null || playerDash == null || playerShooting == null)
        {
            Engineson.print("ERROR: PlayerController is missing required components!");
        }
    }

    public override void Update(float deltaTime)
    {
        Vector3 moveDirection = playerInput.GetCurrentMoveDirection();
        playerMovement.SetMoveDirection(moveDirection);

        if (playerInput.IsDashPressed() && playerDash.CanDash() && moveDirection != Vector3.Zero)
        {
            playerDash.InitiateDash(moveDirection);
        }
    }
}