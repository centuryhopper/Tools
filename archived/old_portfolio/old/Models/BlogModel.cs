using System.ComponentModel.DataAnnotations;

namespace Portfolio.Entities;

public class BlogModel
{
    [Display(Name = "Blog Title:")]
    public string Title { get; set; }
    [DataType(DataType.Date), Display(Name = "Blog Date:")]
    public DateTime Date { get; set; }
    public string? PreviewDesc { get; set; }
    public string? RouteName { get; set; }
    public List<VideoUrlModel>? VideoUrls { get; set; }
    [Display(Name = "Blog Description:")]
    public string FullDesc { get; set; }

}
