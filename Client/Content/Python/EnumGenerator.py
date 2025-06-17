import unreal
import os
import csv
import json
import glob


class enum_data:
    def __init__(self,name,desc):
        self.name = name
        if str(desc)=='nan':
            self.desc = ""
        else:
            self.desc = desc
            
#project name
project_name = ""
enum_folder_path = ""
enum_data_dic = {}
generate_enum_str = ""
enum_save_folder = ""

def next_line(file):
    file.write("\n")

def create_enum():
    global generate_enum_str
    enum_str= ""
    enum_files= glob.glob(os.path.join(enum_folder_path,"*.csv"))
    for file in enum_files:
        if os.path.basename(file).startswith("~$"):
            continue
        print(file)
        with open (file,'r',encoding='utf-8-sig') as csvfile:
            csv_reader = csv.reader(csvfile)
            key = ""
            for row in csv_reader:
                if row[1] == "":
                    continue
                if row[1] == "!Enum":
                    key=row[2]
                    if key not in enum_data_dic:
                        enum_data_dic[key] = []
                if row[1] == "!Enumerator":
                    enum_data_dic[key].append(enum_data(name=row[2],desc=row[4]))

    for key,data_list in enum_data_dic.items():
        enum_str +="UENUM(BlueprintType)\n"
        enum_str +="enum class E"+key+":uint8\n"
        enum_str +="{\n"
        for data in data_list:
            enum_str += "\t"+data.name+", "
            if data.desc != "":
                enum_str += "//"+ data.desc + "\n"
            else:
                enum_str+="\n"
        enum_str+="};\n\n"
    generate_enum_str += str(enum_str)

def create_file():
    with open(enum_save_folder + "/EnumGenerateData.h",'w',encoding="utf-16") as c_file:
        c_file.writelines("# pragma once\n")
        next_line(c_file)
        c_file.writelines("#include \"EnumGenerateData.generated.h\"\n")
        next_line(c_file)
        c_file.writelines(generate_enum_str)
        next_line(c_file)

enum_save_folder= ""

#Load Enum File
json_file_path = unreal.SystemLibrary.get_project_directory() + "/Content/" + project_name + "/Data/Config/Config.json"

if os.path.exists(json_file_path):
    try:
        with open(json_file_path,'r') as file:
            data = json.load(file)
            project_name = unreal.Paths.get_base_filename(unreal.Paths.get_project_file_path())
            enum_save_folder = unreal.SystemLibrary.get_project_directory() + "Source/" + project_name + "/Data"
            enum_folder_path = data["EnumFolderPath"]
    except json.JSONDecodeError as e:
        print("Json Load Faile : {e}")

print("Start Generate Enum File")
create_enum()
create_file()