using Business.DTOs;

namespace Portfolio.Repositories;

public interface IContactsDataRepository<T>
{
    Task<ServiceResponse> PostDataAsync(T model);
    Task<IEnumerable<T>> GetContactsAsync();
}