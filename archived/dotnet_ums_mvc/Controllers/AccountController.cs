using DotnetUserManagementSystem.Models;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using DotnetUserManagementSystem.Contexts;
using DotnetUserManagementSystem.Utilities;
using System.Security.Claims;
using Microsoft.AspNetCore.Authorization;
using DotnetUserManagementSystem.Entities;

namespace DotnetUserManagementSystem.Controllers
{
    public class AccountController : Controller
    {
        private readonly UserManager<ApplicationUser> userManager;
        private readonly SignInManager<ApplicationUser> signInManager;
        private readonly RoleManager<ApplicationRole> roleManager;
        private readonly IConfiguration configuration;
        private readonly IHostEnvironment env;

        public AccountController(UserManager<ApplicationUser> userManager,
            SignInManager<ApplicationUser> signInManager, RoleManager<ApplicationRole> roleManager, IConfiguration configuration, IHostEnvironment env)
        {
            this.userManager = userManager;
            this.signInManager = signInManager;
            this.roleManager = roleManager;
            this.configuration = configuration;
            this.env = env;
        }

        [AllowAnonymous]
        public IActionResult ChangePassword()
        {
            return View();
        }

        [HttpGet]
        [AllowAnonymous]
        public IActionResult ResetPassword(string token, string email)
        {
            // If password reset token or email is null, most likely the
            // user tried to tamper the password reset link
            if (token == null || email == null)
            {
                ModelState.AddModelError("", "Invalid password reset token");
            }

            return View();
        }

        [HttpPost]
        [AllowAnonymous]
        public async Task<IActionResult> ResetPassword(ResetPasswordVM model)
        {
            if (ModelState.IsValid)
            {
                var user = await userManager.FindByEmailAsync(model.Email);

                if (user != null)
                {
                    var result =
                        await userManager.ResetPasswordAsync(user, model.Token, model.Password);
                    if (result.Succeeded)
                    {
                        // Upon successful password reset and if the account is lockedout, set
                        // the account lockout end date to current UTC date time, so the user
                        // can login with the new password
                        if (await userManager.IsLockedOutAsync(user))
                        {
                            await userManager.SetLockoutEndDateAsync(user, DateTimeOffset.UtcNow);
                        }
                        return View("ResetPasswordConfirmation");
                    }

                    foreach (var error in result.Errors)
                    {
                        ModelState.AddModelError("", error.Description);
                    }
                    return View(model);
                }

                return View("ResetPasswordConfirmation");
            }
            return View(model);
        }

        public ActionResult Login()
        {
            return View();
        }

        [HttpPost]
        public async Task<ActionResult> Login(LoginVM vm, string? returnUrl)
        {
            if (ModelState.IsValid)
            {
                var result = await signInManager.PasswordSignInAsync(
                    vm.Email, vm.Password, vm.RememberMe, lockoutOnFailure: true);

                if (result.IsLockedOut)
                {
                    return View("AccountLockout");
                }

                if (result.Succeeded)
                {
                    if (!string.IsNullOrEmpty(returnUrl))
                    {
                        return LocalRedirect(returnUrl);
                    }
                    return RedirectToAction(nameof(HomeController.Index), "Home");
                }

                // ModelState.AddModelError(string.Empty, "Invalid Login Attempt");
                TempData[TempDataKeys.ALERT_ERROR] = "Invalid login attempt!";
            }
            return View(vm);
        }

        public ActionResult Register()
        {
            return View();
        }

        public async Task<IActionResult> ConfirmEmail(string token, string userId)
        {
            if (string.IsNullOrEmpty(token) || string.IsNullOrEmpty(userId))
            {
                TempData[TempDataKeys.ALERT_ERROR] = "Missing user id or token.";
                return RedirectToAction(nameof(HomeController.Index), "Home");
            }

            var user = await userManager.FindByIdAsync(userId);

            if (user is null)
            {
                TempData[TempDataKeys.ALERT_ERROR] = "The user id is invalid";
                return RedirectToAction(nameof(HomeController.Index), "Home");
            }

            var result = await userManager.ConfirmEmailAsync(user, token);

            if (result.Succeeded)
            {
                TempData[TempDataKeys.ALERT_SUCCESS] = "The email has been successfully confirmed!";
                user.UserName = user.Email;
                user.NormalizedUserName = user.UserName.ToUpper();
                await userManager.UpdateAsync(user);
                return RedirectToAction(nameof(HomeController.Index), "Home");
            }

            TempData[TempDataKeys.ALERT_ERROR] = "We couldn't confirm your email.";
            return RedirectToAction(nameof(HomeController.Index), "Home");
        }

        private async Task<IdentityResult> CreateRole(string roleName)
        {
            // We just need to specify a unique role name to create a new role
            ApplicationRole role = new ApplicationRole
            {
                Name = roleName
            };

            // Saves the role in the underlying AspNetRoles table
            IdentityResult result = await roleManager.CreateAsync(role);

            return result;
        }

        [HttpPost]
        public async Task<ActionResult> Register(RegisterVM vm)
        {
            if (ModelState.IsValid)
            {
                // Copy data from RegisterViewModel to IdentityUser
                var user = new ApplicationUser
                {
                    UserName = vm.Email,
                    Email = vm.Email
                };

                var roleToCreate = Constants.USER;

                if (!await roleManager.RoleExistsAsync(roleToCreate))
                {
                    var createRole = await CreateRole(roleToCreate);
                    if (!createRole.Succeeded)
                    {
                        foreach (var error in createRole.Errors)
                        {
                            ModelState.AddModelError(string.Empty, error.Description);
                        }

                        return View(vm);
                    }
                }


                // Store user data in AspNetUsers database table
                var result = await userManager.CreateAsync(user, vm.Password);

                // If user is successfully created, sign-in the user using
                // SignInManager and redirect to index action of HomeController
                if (result.Succeeded)
                {
                    if (!await userManager.IsInRoleAsync(user, Constants.USER))
                    {
                        var addRoleResult = await userManager.AddToRoleAsync(user, Constants.USER);
                        if (!addRoleResult.Succeeded)
                        {
                            foreach (var error in addRoleResult.Errors)
                            {
                                ModelState.AddModelError(string.Empty, error.Description);
                            }

                            return View(vm);
                        }
                    }

                    if (userManager.Options.SignIn.RequireConfirmedEmail)
                    {
                        var emailConfirmToken = await userManager.GenerateEmailConfirmationTokenAsync(user);

                        var confirmationLink = Url.Action("ConfirmEmail", "Account", new { userId = user.Id, token = emailConfirmToken }, Request.Scheme);

                        var smtpInfo = env.IsDevelopment() ? configuration.GetConnectionString("smtp_client").Split("|") : Environment.GetEnvironmentVariable("smtp_client").Split("|");

                        Helpers.SendEmail(subject: "confirm email", senderEmail: smtpInfo[0], senderPassword: smtpInfo[1], body: confirmationLink, receivers: [user.Email]);

                        TempData[TempDataKeys.ALERT_SUCCESS] = "Registration Successful! Please confirm your email to login.";
                    }
                    else
                    {
                        await signInManager.SignInAsync(user, isPersistent: false);
                    }

                    return RedirectToAction(nameof(HomeController.Index), "Home");
                }

                // If there are any errors, add them to the ModelState object
                // which will be displayed by the validation summary tag helper
                foreach (var error in result.Errors)
                {
                    ModelState.AddModelError(string.Empty, error.Description);
                }
            }

            return View(vm);
        }

        [HttpGet]
        [AllowAnonymous]
        public IActionResult ForgotPassword()
        {
            return View();
        }

        [HttpPost]
        [AllowAnonymous]
        public async Task<IActionResult> ForgotPassword(ForgotPasswordVM vm)
        {
            if (ModelState.IsValid)
            {
                // Find the user by email
                var user = await userManager.FindByEmailAsync(vm.Email);
                // If the user is found AND Email is confirmed
                if (user != null && await userManager.IsEmailConfirmedAsync(user))
                {
                    // Generate the reset password token
                    var token = await userManager.GeneratePasswordResetTokenAsync(user);

                    // Build the password reset link
                    var passwordResetLink = Url.Action("ResetPassword", "Account",
                            new { email = vm.Email, token = token }, Request.Scheme);

                    var smtpInfo = env.IsDevelopment() ? configuration.GetConnectionString("smtp_client").Split("|") : Environment.GetEnvironmentVariable("smtp_client").Split("|");

                    Helpers.SendEmail(subject: "confirm email", senderEmail: smtpInfo[0], senderPassword: smtpInfo[1], body: passwordResetLink, receivers: [vm.Email]);

                    // Log the password reset link
                    // logger.Log(LogLevel.Warning, passwordResetLink);

                    TempData[TempDataKeys.ALERT_SUCCESS] = "If you have an account with us, we have sent an email with the instructions to reset your password.";
                    return RedirectToAction(nameof(HomeController.Index), "Home");
                }

                TempData[TempDataKeys.ALERT_SUCCESS] = "If you have an account with us, we have sent an email with the instructions to reset your password.";
                return RedirectToAction(nameof(HomeController.Index), "Home");
            }

            TempData[TempDataKeys.ALERT_SUCCESS] = "If you have an account with us, we have sent an email with the instructions to reset your password.";
            return View(vm);
        }


        public async Task<ActionResult> Logout()
        {
            await signInManager.SignOutAsync();
            return RedirectToAction(nameof(HomeController.Index), "Home");
        }

        public IActionResult AccessDenied()
        {
            return View();
        }

        public async Task<IActionResult> Profile()
        {
            var myId = User.FindFirstValue(ClaimTypes.NameIdentifier);
            var me = await userManager.FindByIdAsync(myId!);

            return View(new ProfileVM
            {
                CurrentEmail = me!.Email!,
            });
        }

        [HttpPost]
        public async Task<IActionResult> Profile(ProfileVM vm)
        {
            if (!ModelState.IsValid)
            {
                var modelStateErrors = Helpers.GetErrors<AccountController>(ModelState);
                TempData[TempDataKeys.ALERT_ERROR] = string.Join("$$$", modelStateErrors);
                return View(vm);
            }

            var user = await userManager.GetUserAsync(User);
            if (user == null)
            {
                return RedirectToAction("Login");
            }

            List<string> errors = [];
            List<string> successes = [];

            if (!string.IsNullOrEmpty(vm.CurrentEmail) && vm.CurrentEmail.ToLower() != user.Email!.ToLower())
            {
                // Change the email
                var token = await userManager.GenerateChangeEmailTokenAsync(user, vm.CurrentEmail);
                var result = await userManager.ChangeEmailAsync(user, vm.CurrentEmail, token);

                bool emailChanged = false;

                if (result.Succeeded)
                {
                    if (userManager.Options.SignIn.RequireConfirmedEmail)
                    {
                        var confirmationToken = await userManager.GenerateEmailConfirmationTokenAsync(user);
                        var confirmationLink = Url.Action("ConfirmEmail", "Account", new { userId = user.Id, token = confirmationToken }, Request.Scheme);

                        var smtpInfo = env.IsDevelopment() ? configuration.GetConnectionString("smtp_client").Split("|") : Environment.GetEnvironmentVariable("smtp_client").Split("|");

                        Helpers.SendEmail(subject: "confirm email", senderEmail: smtpInfo[0], senderPassword: smtpInfo[1], body: confirmationLink, receivers: [vm.CurrentEmail]);

                        successes.Add("Email updated! Please confirm it to login.");
                    }
                    else
                    {
                        emailChanged = true;
                        successes.Add("Email updated!");
                    }
                }
                else
                {
                    errors.Add("Couldn't update your email.");
                }

                if (emailChanged)
                {
                    user.UserName = user.Email;
                    user.NormalizedUserName = user.UserName.ToUpper();
                    await userManager.UpdateAsync(user);
                }
            }



            if (!string.IsNullOrEmpty(vm.NewPassword))
            {
                // ChangePasswordAsync changes the user password
                var result = await userManager.ChangePasswordAsync(user,
                    vm.CurrentPassword, vm.NewPassword);

                // The new password did not meet the complexity rules or
                // the current password is incorrect. Add these errors to
                // the ModelState and rerender ChangePassword view
                if (!result.Succeeded)
                {
                    foreach (var error in result.Errors)
                    {
                        // ModelState.AddModelError(string.Empty, error.Description);
                        errors.Add(error.Description);
                    }

                    // return View(vm);
                }

                // Upon successfully changing the password refresh sign-in cookie
                await signInManager.RefreshSignInAsync(user);

                // TempData[TempDataKeys.ALERT_SUCCESS] = "We have successfully changed your password!";
                successes.Add("We have successfully changed your password!");
                // return View(vm);
            }

            if (successes.Any())
            {
                TempData[TempDataKeys.ALERT_SUCCESS] = string.Join("$$$", successes);
            }
            if (errors.Any())
            {
                TempData[TempDataKeys.ALERT_ERROR] = string.Join("$$$", errors);
            }

            if (errors.Count == 0 && successes.Count == 0)
            {
                TempData[TempDataKeys.ALERT_WARNING] = "No changes made";
            }

            return View(vm);
        }
    }
}
