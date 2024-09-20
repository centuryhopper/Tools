namespace Portfolio.Entities;


public class ProjectCardModel
{
    public string? ImgUrl { get; set; }
    public string? Title { get; set; }
    public string? Description { get; set; }
    public string? ProjectLink { get; set; }
    public string? SourceCodeLink { get; set; }

    public override string ToString()
    {
        return $"{nameof(ImgUrl)}:{ImgUrl}, {nameof(Title)}:{Title}, {nameof(Description)}:{Description}, {nameof(ProjectLink)}:{ProjectLink}, {nameof(SourceCodeLink)}:{SourceCodeLink}, ";
    }

}