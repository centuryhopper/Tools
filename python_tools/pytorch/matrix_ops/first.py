import torch
from pprint import pprint

# print("PyTorch version:", torch.__version__)
# print("CUDA available:", torch.cuda.is_available())

# if torch.cuda.is_available():
#     print("GPU:", torch.cuda.get_device_name(0))


def element_wise_mult():
    a = torch.tensor([[1, 2], [3, 4]])
    b = torch.tensor([[10, 20], [30, 40]])
    
    '''
    [1,2]  [10,20]   =>   [10,40]
    [3,4]  [30,40]   =>   [90,160]
    '''
    element_wise_product = a * b
    print(f"Element-wise multiplication result: {element_wise_product}\n")

def matrix_mult():
    m1 = torch.tensor([[1, 2, 3], [4, 5, 6]])
    m2 = torch.tensor([[7, 8], [9, 10], [11, 12]])
    
    '''
    2 x 3 @  3 x 2  =>  2 x 2
                [7, 8]         [1*7 + 2*9 + 3*11 = 58,  1*8 + 2*10 + 3*12 = 64]
    [1,2,3]     [9, 10]    =>  [4*7 + 5*9 + 6*11 = 139, 4*8 + 5*10 + 6*12 = 154]
    [4,5,6]     [11, 12]
    '''
    matrix_product = m1 @ m2    
    print(f"Matrix 1 (shape {m1.shape}):\n {m1}\n")
    print(f"Matrix 2 (shape {m2.shape}):\n {m2}\n")
    print(f"Matrix Product (m1 @ m2):\n {matrix_product}")
    
if __name__ == "__main__":
    # element_wise_mult()
    # matrix_mult()
    
    m1 = torch.randn(1,1)
    print(m1)
    