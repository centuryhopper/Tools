
using Newtonsoft.Json;

namespace dotnet_bmbl_bot.Models;

public class AccountConfig
{
    public string AboutMe { get; set; }
    public Prompt Prompt1 { get; set; }
    public Prompt Prompt2 { get; set; }
    public Prompt Prompt3 { get; set; }
    
    [JsonProperty("job_title")]
    public string JobTitle { get; set; }
    public string Company { get; set; }
    public string Institution { get; set; }
    public string Latitude { get; set; }
    public string longitude { get; set; }
    
    
}