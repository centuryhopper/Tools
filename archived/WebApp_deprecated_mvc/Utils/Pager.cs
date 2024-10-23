using Microsoft.AspNetCore.Mvc.RazorPages;



namespace LeoPasswordManager.Utilities;



public class Pager
{

    public int TotalItems { get; set; }

    public int CurrentPage { get; set; }

    public int PageSize { get; set; }

    public int TotalPages { get; set; }

    public int StartPage { get; set; }

    public int EndPage { get; set; }



    public Pager()
    {

    }



    public Pager(int totalItems, int pageNumber, int pageSize)
    {

        int totalPages = (int)Math.Ceiling((decimal)totalItems / (decimal)pageSize);

        int currentPage = pageNumber;

        // limit the range of pagination
        int startPage = currentPage - 5;
        int endPage = currentPage + 4;

        if (startPage <= 0)
        {
            startPage = 1;
            endPage = endPage - (startPage - 1);
        }



        if (endPage > totalPages)
        {
            endPage = totalPages;

            if (endPage > 10)
            {

                startPage = endPage - 9;

            }
        }

        TotalItems = totalItems;
        CurrentPage = currentPage;
        PageSize = pageSize;
        TotalPages = totalPages;
        StartPage = startPage;
        EndPage = endPage;
    }



}