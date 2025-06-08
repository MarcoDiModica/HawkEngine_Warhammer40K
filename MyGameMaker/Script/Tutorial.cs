using HawkEngine;
using System;
using System.Numerics;

public class Tutorial : MonoBehaviour
{
    private GameObject Move;
    private GameObject Aim;
    private GameObject Shoot;
    private GameObject Dash;
    private GameObject Reload;
    private GameObject SwitchWeapon;
    private GameObject WeaponAbility;
    private UIImage moveImage;
    private UIImage aimImage;
    private UIImage shootImage;
    private UIImage dashImage;
    private UIImage reloadImage;
    private UIImage switchWeaponImage;
    private UIImage weaponAbilityImage;
    float timer = 0.0f;



    public override void Awake()
    {
        //Engineson.print("WinScreen Awake");
    }

    public override void Start()
    {
        Move = GameObject.Find("Move");
        Aim = GameObject.Find("Aim");
        Shoot = GameObject.Find("Shoot");
        Dash = GameObject.Find("Dash");
        Reload = GameObject.Find("Reload");
        SwitchWeapon = GameObject.Find("SwitchWeapon");
        WeaponAbility = GameObject.Find("WeaponAbility");
        if (Move == null || Aim == null || Shoot == null || Dash == null || Reload == null || SwitchWeapon == null || WeaponAbility == null)
        {
            Engineson.print("One or more tutorial objects not found!");
        }
        else
        {
            Engineson.print("All tutorial objects initialized successfully.");
        }
        moveImage = Move.GetComponent<UIImage>();
        aimImage = Aim.GetComponent<UIImage>();
        shootImage = Shoot.GetComponent<UIImage>();
        dashImage = Dash.GetComponent<UIImage>();
        reloadImage = Reload.GetComponent<UIImage>();
        switchWeaponImage = SwitchWeapon.GetComponent<UIImage>();
        weaponAbilityImage = WeaponAbility.GetComponent<UIImage>();
        if (moveImage == null || aimImage == null || shootImage == null || dashImage == null || reloadImage == null || switchWeaponImage == null || weaponAbilityImage == null)
        {
            Engineson.print("One or more tutorial images not found!");
        }
        else
        {
            Engineson.print("All tutorial images initialized successfully.");
        }
        moveImage.SetImageHasAnimation(true);
        moveImage.SetImageAnimationSpeed(0.5f);
        moveImage.SetImageSpriteSize(890.0f, 538.0f);
        moveImage.SetImageAnimationIndexLimit(3);
        moveImage.SetImageAnimation(0);
        aimImage.SetImageHasAnimation(true);
        aimImage.SetImageAnimationSpeed(0.5f);
        aimImage.SetImageSpriteSize(890.0f, 538.0f);
        aimImage.SetImageAnimationIndexLimit(3);
        aimImage.SetImageAnimation(0);
        shootImage.SetImageHasAnimation(true);
        shootImage.SetImageAnimationSpeed(0.5f);
        shootImage.SetImageSpriteSize(890.0f, 538.0f);
        shootImage.SetImageAnimationIndexLimit(3);
        shootImage.SetImageAnimation(0);
        dashImage.SetImageHasAnimation(true);
        dashImage.SetImageAnimationSpeed(0.5f);
        dashImage.SetImageSpriteSize(890.0f, 538.0f);
        dashImage.SetImageAnimationIndexLimit(3);
        dashImage.SetImageAnimation(0);
        reloadImage.SetImageHasAnimation(true);
        reloadImage.SetImageAnimationSpeed(0.5f);
        reloadImage.SetImageSpriteSize(890.0f, 538.0f);
        reloadImage.SetImageAnimationIndexLimit(3);
        reloadImage.SetImageAnimation(0);
        switchWeaponImage.SetImageHasAnimation(true);
        switchWeaponImage.SetImageAnimationSpeed(0.5f);
        switchWeaponImage.SetImageSpriteSize(890.0f, 538.0f);
        switchWeaponImage.SetImageAnimationIndexLimit(3);
        switchWeaponImage.SetImageAnimation(0);
        weaponAbilityImage.SetImageHasAnimation(true);
        weaponAbilityImage.SetImageAnimationSpeed(0.5f);
        weaponAbilityImage.SetImageSpriteSize(890.0f, 538.0f);
        weaponAbilityImage.SetImageAnimationIndexLimit(3);
        weaponAbilityImage.SetImageAnimation(0);
        Move.SetActive(true);
    }

    public override void Update(float deltaTime)
    {
        timer += deltaTime;
        if (timer> 3.0f && Move.IsActive())
        {
            Move.SetActive(false);
            Aim.SetActive(true);
        }
        if (timer > 6.0f && Aim.IsActive())
        {
            Aim.SetActive(false);
            Shoot.SetActive(true);
        }
        if (timer > 9.0f && Shoot.IsActive())
        {
            Shoot.SetActive(false);
            Dash.SetActive(true);
        }
        if (timer > 12.0f && Dash.IsActive())
        {
            Dash.SetActive(false);
            Reload.SetActive(true);
        }
        if (timer > 15.0f && Reload.IsActive())
        {
            Reload.SetActive(false);
            SwitchWeapon.SetActive(true);
        }
        if (timer > 18.0f && SwitchWeapon.IsActive())
        {
            SwitchWeapon.SetActive(false);
            WeaponAbility.SetActive(true);
        }
        if (timer > 21.0f && WeaponAbility.IsActive())
        {
            WeaponAbility.SetActive(false);
            gameObject.SetActive(false);
        }

    }
}
