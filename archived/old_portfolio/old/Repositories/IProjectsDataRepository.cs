using Business.DTOs;

namespace Portfolio.Repositories;


public interface IProjectsDataRepository<T>
{
    Task<IEnumerable<T>> GetDataAsync(string? searchTerm);
    Task<ServiceResponse> AddProjectAsync(T model);
}