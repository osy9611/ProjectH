import unreal
import json
import os
import sys
import csv


def create_tabledatas():
    project_name = unreal.Paths.get_base_filename(unreal.Paths.get_project_file_path())
    #LoadJson
    json_file_path = unreal.SystemLibrary.get_project_directory() + "/Content/Data/Config/Config.json"
    print(project_name)
    print(json_file_path)

    if os.path.exists(json_file_path):
        try:
            with open(json_file_path,'r') as file:
                data = json.load(file)
                project_name = unreal.Paths.get_base_filename(unreal.Paths.get_project_file_path())
                csv_folder = data["CSVFolderPath"]
                print(csv_folder)
        except json.JSONDecodeError as e:
            print("Json Load Faile : {e}")
   
    print("Start Generate CSV File")
    file_list = os.listdir(csv_folder)
    for file in file_list:
        if file.endswith(".csv"):
            file_name = str(file).split('.')[0]
            csv_file_path = os.path.join(csv_folder,file)
            ustruct_name = f'/Script/{project_name}.{file_name}'
            data_table_name = f"DT_{file_name}"
            asset_folder = "/Game/Data/Data"
            create_tabledata(csv_file_path, ustruct_name, data_table_name, asset_folder)

def create_tabledata(csv_file_path, ustruct_name, data_table_name, asset_folder):
    #에셋 툴즈 라이브러리 인스턴스 생성
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    #데이터 테이블 팩토리 생성
    data_table_factory = unreal.DataTableFactory()
    data_table_factory.struct = unreal.load_object(None, ustruct_name)

    if not data_table_factory.struct:
        unreal.log_error(f"USTRUCT {ustruct_name}을 찾을 수 없습니다. 경로 및 이름을 확인해주세요")
        return
    
    #CSV를 추출해서 순 데이터만 존재하는 임시파일 생성
    origin_rows=[]
    tarray_columns = [] 
    type_columns = []
    # with open(csv_file_path,'r') as origin:
    #     csv_reader = csv.reader(origin)
    #     id_row_index = sys.maxsize

    #     for index,row in enumerate(csv_reader):
    #         if index == 1:
    #             continue
    #         origin_rows.append(row)
    #     print(origin_rows)

    with open(csv_file_path,'r') as origin:
        csv_reader = csv.reader(origin)
        id_row_index = sys.maxsize

        for index,row in enumerate(csv_reader):
            if index == 1:
                for idx, cell in enumerate(row):
                    cell_stripped = cell.strip().lower()
                    type_columns.append(cell_stripped)
                    if cell_stripped.startswith("tarray<") and cell_stripped.endswith(">"):
                        tarray_columns.append(idx)
                        print(f"TArray type found in column {idx}: {cell}")
                continue
            origin_rows.append(row)

    origin_rows[0][0] = "---"
    print(origin_rows)
    temp_csv_path = unreal.SystemLibrary.get_project_directory() + "Temp_TableGenerator.csv"

    # with open(temp_csv_path,'w') as temp_csv:
    #     writer = csv.writer(temp_csv, quoting=csv.QUOTE_MINIMAL)
    #     for row in origin_rows:
    #         for index,data in enumerate(row):
    #             if index in tarray_columns:
    #                 if origin_rows[0] != row:
    #                     print(row)
    #                     data = "("+data+")"
    #                     #data = "(1,0,0)"
    #                 if (',' in data) or ('(' in data) or (')' in data):
    #                     data = f'"{data}"'
                    
    #                 print(type_columns[index])
    #                 print(data)
    #             temp_csv.write(data)
    #             if index != len(row):
    #                 temp_csv.write(",")
    #         temp_csv.write('\n')

    with open(temp_csv_path, 'w', newline='', encoding='utf-8') as temp_csv:
        writer = csv.writer(temp_csv, quoting=csv.QUOTE_MINIMAL)
        writer.writerow(origin_rows[0])
        #writer.writerow(type_columns)
        for row in origin_rows[1:]:
            new_row = list(row)
            for idx in tarray_columns:
                raw = new_row[idx].strip()
                if raw == "":
                    new_row[idx] = "()"
                else:
                    if raw.startswith("(") and raw.endswith(")"):
                         raw = raw[1:-1]
                    parts = [p.strip() for p in raw.split(",") if p.strip()]
                    
                    str_type = str(type_columns[idx]).lower()
                    if str_type == "tarray<fstring>" or str_type == "tarray<string>":
                       new_row[idx] = "(" + ",".join(f'"{p}"' for p in parts) + ")"
                    else:
                       new_row[idx] = "(" + ",".join(f'{p}' for p in parts) + ")"
            writer.writerow(new_row)

    
    csv_faactory = unreal.CSVImportFactory()
    csv_faactory.automated_import_settings.import_row_struct = data_table_factory.struct

    task = unreal.AssetImportTask()
    task.filename = temp_csv_path
    task.destination_name = data_table_name
    task.destination_path = asset_folder
    task.replace_existing = True
    task.automated = True
    task.save = True
    task.factory = csv_faactory

    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

    try:
        os.remove(temp_csv_path)
    except FileNotFoundError:
        return
    except Exception as e:
        unreal.log_error(e)
        
create_tabledatas()