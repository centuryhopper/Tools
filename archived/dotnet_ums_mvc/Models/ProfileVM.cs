using System.ComponentModel.DataAnnotations;
namespace DotnetUserManagementSystem.Models;

public class ProfileVM
{
    public string? Id { get; set; }
    
    [DataType(DataType.EmailAddress)]
    [Display(Name = "Current Email")]
    public string CurrentEmail { get; set; }

    [DataType(DataType.Password)]
    [Display(Name = "Current password")]
    public string? CurrentPassword { get; set; }

    [DataType(DataType.Password)]
    [Display(Name = "New password")]
    public string? NewPassword { get; set; }

    [DataType(DataType.Password)]
    [Display(Name = "Confirm new password")]
    [Compare(nameof(NewPassword), ErrorMessage =
        "The new password and confirmation password do not match.")]
    public string? ConfirmPassword { get; set; }
}
