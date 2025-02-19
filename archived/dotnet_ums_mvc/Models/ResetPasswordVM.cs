
using System.ComponentModel.DataAnnotations;

namespace DotnetUserManagementSystem.Models;

public class ResetPasswordVM
{
    [Required]
    [EmailAddress]
    public string Email { get; set; }

    [Required]
    [DataType(DataType.Password)]
    public string Password { get; set; }

    [DataType(DataType.Password)]
    [Display(Name = "Confirm password")]
    [Compare(nameof(Password),
        ErrorMessage = "Password and Confirm Password must match")]
    public string ConfirmPassword { get; set; }

    public string Token { get; set; }

}
