import unreal
import json
import os
import sys
import csv
import glob
import time


#project name
project_name = ""
#csv path
csv_folder = ""
#enum path
enum_folder = ""
#enum names
enum_names = []
#use live coding
use_live_coding = False
#genrated string
genrated_str = ''

#c++ Struct Save Path
struct_save_folder = ""

def next_line(file):
    file.write("\n")
    
# CSV 셀 값을 처리하는 함수 (TArray 타입의 경우 쉼표를 세미콜론으로 치환)
def process_csv_cell(cell_value, field_type):
    # field_type의 소문자 비교: TArray<...> 인 경우
    if str(field_type).lower().startswith("tarray<") and str(field_type).lower().endswith(">"):
        # 쉼표(,)를 세미콜론(;)으로 치환
        return cell_value.replace(",", ";")
    return cell_value

#type def
def get_unreal_type(type):
    str_type = str(type).lower()
    print(str_type)
    if str_type.startswith("tarray<") and str_type.endswith(">"):
         inner_type_str = str_type[len("tarray<"):-1].strip()
         inner_unreal_type = get_unreal_type(inner_type_str)
         return "TArray<" + inner_unreal_type + ">"

    if str_type == "int" or str_type == "int32":
        return "int32"
    elif str_type == "float" or str_type == "float32":
        return "float"
    elif str_type == "string" or str_type == "fstring":
        return "FString"
    elif str_type == "bool" or str_type == "boolean":
        return "bool"
    elif str_type == "vector" or str_type == "vector3":
        return "FVector"
    elif str_type == "rotator" or str_type == "rotator":
        return "FRotator"
    elif str_type == "text":
        return "FText"
    elif str_type == "color" or str_type == "coloru8":
        return "FLinearColor"
    elif type in enum_names:
        return "E"+type
    else:
        unreal.log_error(str_type + " << This type is not allowed. It will change to \'FString\'.")
        return "FString"

def get_enum_data():
    global enum_names
    enum_files= glob.glob(os.path.join(enum_folder,"*.csv"))
    print("111111")
    for file in enum_files:
        if os.path.basename(file).startswith("~$"):
            continue

        with open(file,'r') as csvfile:
            csv_reader = csv.reader(csvfile)
            key = ""
            for row in csv_reader:
                if row[1] == "":
                    continue
                if row[1] == "!Enum":
                    enum_names.append(str(row[2]))


def create_struct():
    global genrated_str
    print("#######   Data Table C++ Struct Generator Started!     #######")
    print("######    Target CSV Folder : " + csv_folder)
    print("-")

    file_list = os.listdir(csv_folder)

    csv_file_list = []

    for file in file_list:
        if file.endswith(".csv"):
            csv_file_list.append(file)

    if len(csv_file_list) == 0:
        unreal.log_error("Thre's no CSV file in folder : " + path.csv_folder)
        sys.exit(0)

    print("----------- CSV File List ------------")
    print("-")

    index = 1
    for file in csv_file_list:
        print("("+str(index)+") "+file)
        index += 1

    print("-")


    for file in csv_file_list:
        print("-")
        print("::::::::::::: Start making [" + file + "] ::::::::::::::")

        csv_file_path = os.path.join(csv_folder,file)
        print("----------- Writing C++ Struct row Table... ------------")
        print("-")

        rows = []

        with open(csv_file_path, 'r') as csvfile:
            csv_reader = csv.reader(csvfile)
            for row in csv_reader:
                rows.append(row)

        #행이 없다면 종료
        if len(rows) == 0:
            unreal.log_error("CSV row count is 0")
            continue

        column_name_row_index = -1
        for data in rows:        
            #print(data)
            #print(str(data[0]).lower())
            if data[0] == "TableNo" or str(data[0]).lower() == "tableno":
                column_name_row_index = rows.index(data)

        if column_name_row_index == -1:
            unreal.log_error("Cannot found Id column")
            continue

        type_name_list = []
        column_name_list = []

        column_name_row = rows[column_name_row_index]

        #데이터 이름과 변수 타입을 저장
        for index, column_name in enumerate(column_name_row):
            #print(column_name)
            if not column_name.startswith("#"):
                if str(column_name).lower() == "tableno":
                    continue
                type_row = rows[1]
                type_name_list.append(type_row[index])
                column_name_list.append(column_name)
        if len(type_name_list) != len(column_name_list):
            print("Type name count and column name count is not correct :" + len(type_name_list)+"/"+len(column_name_list))
            continue

        #File Name
        file_name = os.path.basename(csv_file_path)
        file_name = str(file_name).split('.')[0]

        struct_str = ""
        struct_str += "USTRUCT(BlueprintType)\n"
        struct_str += "struct F"+file_name+" : public FTableRowBase\n"
        struct_str += "{\n"
        struct_str += "\tGENERATED_USTRUCT_BODY()\n"
        struct_str += "public:\n"

        for index,value in enumerate(column_name_list):
            struct_str+="\tUPROPERTY(EditAnywhere, BlueprintReadWrite)\n"
            struct_str+="\t" + get_unreal_type(type_name_list[index]) + " " + str(value)+ ";\n"
        struct_str+="};\n"

        genrated_str += str(struct_str)

def create_file():
     with open(struct_save_folder + "/GenerateTableData.h",'w') as c_file:
        c_file.writelines("# pragma once\n")
        next_line(c_file)
        c_file.writelines("#include \"Engine/DataTable.h\"\n")
        c_file.writelines("#include \"EnumGenerateData.h\"\n")
        c_file.writelines("#include \"GenerateTableData.generated.h\"\n")
        next_line(c_file)
        c_file.writelines(genrated_str)
        next_line(c_file)

def recompile_and_reload():
    # 빌드 시스템 트리거 (Live Coding 등 설정 필요)
    unreal.SystemLibrary.execute_console_command(None, "LiveCoding.Compile")
    unreal.log("Project modules have been reloaded.")


#Load Json file
json_file_path = unreal.SystemLibrary.get_project_directory() + "/Content/" + project_name + "/Data/Config/Config.json"
print(json_file_path)

if os.path.exists(json_file_path):
    try:
        with open(json_file_path,'r') as file:
            data = json.load(file)
            project_name = unreal.Paths.get_base_filename(unreal.Paths.get_project_file_path())
            struct_save_folder = unreal.SystemLibrary.get_project_directory() + "Source/" + project_name + "/Data"
            csv_folder = data["CSVFolderPath"]
            enum_folder = data["EnumFolderPath"]
            use_live_coding  = data["UseLiveCoding"]
    except json.JSONDecodeError as e:
        print("Json Load Faile : {e}")

print("Start Generate CSV File")

get_enum_data()
create_struct()
create_file()
if use_live_coding ==True:
    recompile_and_reload()