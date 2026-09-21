namespace dotnet_amazon_pricetracker.Models;

public class ProductRecord
{
    public int RowNumber;
    public string DateRecorded;
    public string Description;
    public string PriceText;
    public string RatingText;
    public string NumReviewText;
    public string Url;

    public ProductRecord(int RowNumber
    ,string DateRecorded
    ,string Description
    ,string PriceText
    ,string RatingText
    ,string NumReviewText
    ,string Url)
    {
        this.RowNumber = RowNumber;
        this.DateRecorded = DateRecorded;
        this.Description = Description;
        this.PriceText = PriceText;
        this.RatingText = RatingText;
        this.NumReviewText = NumReviewText;
        this.Url = Url;
    }
}


