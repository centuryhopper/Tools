using LeoPasswordManager.Utilities;

namespace LeoPasswordManager.Models;

public class PaginationParameters
{
    public string ActionName { get; set; }
    public string ControllerName { get; set; }
    public Pager pager { get; set; }

}
