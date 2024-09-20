using Business.DTOs;

namespace Portfolio.Repositories;

public interface IBlogsDataRepository<T>
{
    Task<IEnumerable<T>> SortAsync(bool isNewest);
    Task<IEnumerable<T>> GetBlogDataAsync();
    Task<T> GetBlogByTitleAsync(string title);
    Task<ServiceResponse> AddBlogAsync(T model);
}