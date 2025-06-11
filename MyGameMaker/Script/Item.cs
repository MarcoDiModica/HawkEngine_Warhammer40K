using HawkEngine;
using System;
using System.Numerics;

public class Item : MonoBehaviour, IInteractable
{
    public bool hasInteracted { get; set; }
    public bool isTextVisible = false;

    public string dialogueText = "Esto es parte 1/nAhora parte 2/nFinalmente parte 3/f";
    private int currentDialogueIndex = 0;

    public override void Start() { }
    public override void Awake() { }
    public override void Update(float deltaTime) { }

    public void Interact()
    {
        if (!isTextVisible)
        {
            isTextVisible = true;
            currentDialogueIndex = 0;
        }
    }

    public string GetNextDialogue()
    {
        var segments = dialogueText.Split(new[] { "/n", "/f" }, StringSplitOptions.None);

        while (currentDialogueIndex < segments.Length)
        {
            string segment = segments[currentDialogueIndex++].Trim();

            if (!string.IsNullOrEmpty(segment))
            {
                if (dialogueText.Contains("/f") && currentDialogueIndex >= segments.Length)
                {
                    isTextVisible = false;
                }

                return segment;
            }
        }

        isTextVisible = false;
        return null;
    }
    public bool HasMoreDialogue()
    {
        var segments = dialogueText.Split(new[] { "/n", "/f" }, StringSplitOptions.None);
        return currentDialogueIndex < segments.Length;
    }
}
