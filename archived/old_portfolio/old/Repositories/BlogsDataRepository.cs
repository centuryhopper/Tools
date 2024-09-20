using Business.DTOs;

using Microsoft.EntityFrameworkCore;
using Npgsql.Internal;
using Portfolio.Contexts;
using Portfolio.Entities;

namespace Portfolio.Repositories;

public class BlogsDataRepository : IBlogsDataRepository<BlogModel>
{
    private readonly PortfolioDBContext neondbContext;

    public BlogsDataRepository(PortfolioDBContext neondbContext)
    {
        this.neondbContext = neondbContext;
    }

    public async Task<ServiceResponse> AddBlogAsync(BlogModel model)
    {
        var blog = new Blog
        {
            Title = model.Title,
            Date = model.Date,
            PreviewDesc = model.PreviewDesc,
            RouteName = model.RouteName,
            FullDesc = model.FullDesc,
        };

        try
        {
            await neondbContext.Blogs.AddAsync(blog);
            await neondbContext.SaveChangesAsync();

            int mostRecentInsertedBlogModel = await neondbContext.Blogs.MaxAsync(b => b.Id);

            foreach (var vUrlModel in model.VideoUrls ?? [])
            {
                var videoUrl = new VideoUrl
                {
                    BlogId = mostRecentInsertedBlogModel,
                    Url = vUrlModel.url,
                };
                await neondbContext.VideoUrls.AddAsync(videoUrl);
            }

            await neondbContext.SaveChangesAsync();
            return new ServiceResponse(flag: true, message: "Added my blogs");
        }
        catch (Exception e)
        {
            return new ServiceResponse(flag: false, message: e.Message);
        }
    }

    public async Task<BlogModel> GetBlogByTitleAsync(string title)
    {
        var blog = await neondbContext.Blogs.Where(b => b.Title == title).FirstAsync();

        var videoUrls = await neondbContext.VideoUrls.Where(v => v.BlogId == blog.Id).Select(v => new VideoUrlModel
        {
            url = v.Url
            ,
            title = "test"
        }).ToListAsync();

        return new BlogModel
        {
            Title = blog.Title
            ,
            Date = blog.Date
            ,
            PreviewDesc = blog.PreviewDesc
            ,
            RouteName = blog.RouteName
            ,
            VideoUrls = videoUrls
            ,
            FullDesc = blog.FullDesc
        };
    }

    public async Task<IEnumerable<BlogModel>> GetBlogDataAsync()
    {
        var blogs = await neondbContext.Blogs.Include(b => b.VideoUrls).Select(b =>
            new BlogModel
            {
                Title = b.Title,
                Date = b.Date,
                PreviewDesc = b.PreviewDesc,
                RouteName = b.RouteName,
                FullDesc = b.FullDesc,
                VideoUrls = b.VideoUrls.Select(v => new VideoUrlModel
                {
                    title = "test",
                    url = v.Url
                }).ToList(),
            }).ToListAsync();

        return blogs;
    }

    public async Task<IEnumerable<BlogModel>> SortAsync(bool isNewest)
    {
        var blogs = await neondbContext.Blogs.Include(b => b.VideoUrls).Select(b =>
            new BlogModel
            {
                Title = b.Title,
                Date = b.Date,
                PreviewDesc = b.PreviewDesc,
                RouteName = b.RouteName,
                FullDesc = b.FullDesc,
                VideoUrls = b.VideoUrls.Select(v => new VideoUrlModel
                {
                    title = "test",
                    url = v.Url
                }).ToList(),
            }).ToListAsync();

        return isNewest ?
        blogs.OrderByDescending(b => b.Date) :
        blogs.OrderBy(b => b.Date);
    }
}



