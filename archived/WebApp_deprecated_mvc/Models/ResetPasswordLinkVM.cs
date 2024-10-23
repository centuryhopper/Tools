
using System.ComponentModel.DataAnnotations;

namespace LeoPasswordManager.Models;

public class ResetPasswordLinkVM
{
    [Display(Name = "Email"), Required(ErrorMessage = "Please enter your email")]
    public string Email { get; set; }
}