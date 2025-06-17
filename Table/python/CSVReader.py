import os
import csv
import json
import glob
import pandas

#current path
current_path = os.getcwd()

print("Start Generate CSV File")

#엑셀 폴더 경로
excel_folder_path = current_path+"\Datas"

print (excel_folder_path)

#엑셀 폴더에 있는 모든 파일 가져오기
excel_files = glob.glob(os.path.join(excel_folder_path, "*.xlsx"))

for file in excel_files:    
    data_frame = []
    df = pandas.read_excel(file)
    for index,colum in enumerate(df.columns):
        if df[colum].iloc[0] == "!Field":
            if df[colum].iloc[1] == "!All":
                data_frame.append(index)
        else:
            continue

    #조건에 맞는 열들만 포함한 데이터 프레임 생성
    filtered_df = df.iloc[:,data_frame]

    #각 열에서 첫 두 행을 제외한 나머지 행만 남김
    filtered_df = filtered_df.drop([0,1])
    
    # CSV 파일로 저장 
    csv_filename = os.path.splitext(os.path.basename(file))[0]
    csv_file_path = os.path.join(current_path+"\CSV", f"{csv_filename}.csv") 
    filtered_df.to_csv(csv_file_path, index=False, header=False)

excel_folder_path = current_path+"\Enum"
excel_files = glob.glob(os.path.join(excel_folder_path, "*.xlsx"))
for file in excel_files:    
    if os.path.basename(file).startswith("~$"):
        continue
   
    df = pandas.read_excel(file)
    csv_filename = os.path.splitext(os.path.basename(file))[0]
    csv_file_path = os.path.join(current_path+"\Enum", f"{csv_filename}.csv") 
    df.to_csv(csv_file_path, index=False, header=False,encoding='utf-8-sig')

print("Complete CSV Generate!")