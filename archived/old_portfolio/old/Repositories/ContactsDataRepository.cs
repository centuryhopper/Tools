using Business.DTOs;

using Microsoft.EntityFrameworkCore;
using Portfolio.Contexts;
using Portfolio.Entities;

namespace Portfolio.Repositories;

public class ContactsDataRepository : IContactsDataRepository<ContactMeModel>
{
    private readonly PortfolioDBContext neondbContext;

    public ContactsDataRepository(PortfolioDBContext neondbContext)
    {
        this.neondbContext = neondbContext;
    }

    public async Task<IEnumerable<ContactMeModel>> GetContactsAsync()
    {
        var contacts = await neondbContext.Contacttables.AsNoTracking().ToListAsync();
        return contacts.Select(c => new ContactMeModel
        {
            Name = c.Name,
            Message = c.Message,
            Email = c.Email,
            Subject = c.Subject,
        });
    }

    public async Task<ServiceResponse> PostDataAsync(ContactMeModel model)
    {
        var contactTable = new Contacttable
        {
            Name = model.Name!
                ,
            Email = model.Email!
                ,
            Subject = model.Subject!
                ,
            Message = model.Message!
        };

        try
        {
            await neondbContext.Contacttables.AddAsync(contactTable);
            await neondbContext.SaveChangesAsync();
            return new ServiceResponse(flag: true, message: "Added user's message");
        }
        catch (Exception _)
        {
            return new ServiceResponse(flag: false, message: "Error adding user's message");
        }
    }

}



