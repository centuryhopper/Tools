using Business.DTOs;

namespace Portfolio.Repositories;

public interface ISkillsDataRepository<T>
{
    Task<IEnumerable<T>> GetDataAsync();
    Task<ServiceResponse> AddSkillsAsync(T model);
}