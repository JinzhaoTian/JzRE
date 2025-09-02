#!/usr/bin/env python3
"""
JzRE反射工具 - 类似于UnrealHeaderTool的C++反射系统生成器

功能：
1. 预处理阶段：收集所有需要处理的头文件
2. 解析阶段：使用Clang AST遍历解析源文件  
3. 提取阶段：收集类型信息、属性元数据和函数签名
4. 验证阶段：检查反射声明的一致性
5. 生成阶段：产生.generated.h和.generated.cpp文件
"""

import sys
import os
import argparse
import json
import re
import platform
import subprocess
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple, Union
from dataclasses import dataclass, field
from enum import Enum

# 尝试导入clang库
try:
    import clang.cindex
    from clang.cindex import CursorKind, TypeKind, AccessSpecifier, TranslationUnit
    CLANG_AVAILABLE = True
    
    # 尝试自动检测和配置libclang路径
    def setup_libclang():
        """自动检测和设置libclang库路径"""
        system = platform.system().lower()
        possible_paths = []
        
        if system == "windows":
            # Windows常见路径
            possible_paths = [
                "C:/Program Files/LLVM/bin/libclang.dll",
                "C:/Program Files (x86)/LLVM/bin/libclang.dll",
                "C:/LLVM/bin/libclang.dll",
            ]
            # 尝试通过where命令找到clang
            try:
                result = subprocess.run(["where", "clang"], capture_output=True, text=True)
                if result.returncode == 0:
                    clang_path = result.stdout.strip().split('\n')[0]
                    libclang_path = Path(clang_path).parent / "libclang.dll"
                    if libclang_path.exists():
                        possible_paths.insert(0, str(libclang_path))
            except:
                pass
                
        elif system == "darwin":  # macOS
            possible_paths = [
                "/usr/local/opt/llvm/lib/libclang.dylib",
                "/opt/homebrew/opt/llvm/lib/libclang.dylib",
                "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/libclang.dylib",
                "/Library/Developer/CommandLineTools/usr/lib/libclang.dylib",
            ]
        else:  # Linux
            possible_paths = [
                "/usr/lib/llvm-15/lib/libclang.so.1",
                "/usr/lib/llvm-14/lib/libclang.so.1",
                "/usr/lib/llvm-13/lib/libclang.so.1",
                "/usr/lib/llvm-12/lib/libclang.so.1",
                "/usr/lib/x86_64-linux-gnu/libclang-15.so.1",
                "/usr/lib/x86_64-linux-gnu/libclang-14.so.1",
                "/usr/lib/x86_64-linux-gnu/libclang-13.so.1",
                "/usr/lib/libclang.so.1",
                "/usr/lib/libclang.so",
            ]
        
        # 尝试每个路径
        for path in possible_paths:
            if os.path.exists(path):
                try:
                    clang.cindex.conf.set_library_file(path)
                    # 测试是否能正常工作
                    index = clang.cindex.Index.create()
                    print(f"成功配置libclang: {path}")
                    return True
                except Exception as e:
                    print(f"尝试libclang路径失败 {path}: {e}")
                    continue
        
        print("警告: 无法自动检测libclang路径，将使用系统默认配置")
        return False
    
    # 自动设置libclang
    setup_libclang()
    
except ImportError as e:
    print(f"警告: 未找到libclang，请安装: pip install libclang")
    print(f"错误详情: {e}")
    CLANG_AVAILABLE = False

class ReflectionPhase(Enum):
    """反射处理阶段"""
    PREPROCESS = "预处理"
    PARSE = "解析"
    EXTRACT = "提取"
    VALIDATE = "验证"
    GENERATE = "生成"

@dataclass
class PropertyInfo:
    """属性信息"""
    name: str
    type_name: str
    display_name: str = ""
    category: str = ""
    tooltip: str = ""
    is_public: bool = True
    file_path: str = ""
    line_number: int = 0

@dataclass
class MethodInfo:
    """方法信息"""
    name: str
    return_type: str
    parameters: List[Tuple[str, str]] = field(default_factory=list)  # (type, name) pairs
    display_name: str = ""
    category: str = ""
    is_public: bool = True
    file_path: str = ""
    line_number: int = 0

@dataclass
class ClassInfo:
    """类信息"""
    name: str
    parent_name: str = ""
    display_name: str = ""
    category: str = ""
    properties: List[PropertyInfo] = field(default_factory=list)
    methods: List[MethodInfo] = field(default_factory=list)
    file_path: str = ""
    line_number: int = 0
    namespace: str = "JzRE"

class ReflectionParser:
    """反射解析器"""
    
    def __init__(self, source_dir: str, output_dir: str):
        self.source_dir = Path(source_dir)
        self.output_dir = Path(output_dir)
        self.classes: Dict[str, ClassInfo] = {}
        self.processed_files: Set[str] = set()
        
        # 选项配置
        self.verbose = False
        self.force_regenerate = False  
        self.single_file_mode = None
        
        # 反射宏模式 - 支持跨行匹配
        self.jzclass_pattern = re.compile(r'JZCLASS\s*\((.*?)\)', re.MULTILINE | re.DOTALL)
        # 支持跨行的JZPROPERTY模式：JZPROPERTY(...)\n    Type varName = value;
        self.jzproperty_pattern = re.compile(r'JZPROPERTY\s*\([^)]*(?:\([^)]*\))*[^)]*\)\s*(\w+(?:\s*\*|\s*&)*)\s+(\w+)(?:\s*=[^;]*)?;', re.MULTILINE | re.DOTALL)
        # 支持跨行的JZMETHOD模式：JZMETHOD(...)\n    ReturnType MethodName(params);
        self.jzmethod_pattern = re.compile(r'JZMETHOD\s*\([^)]*(?:\([^)]*\))*[^)]*\)\s*(\w+(?:\s*\*|\s*&)*)\s+(\w+)\s*\(([^)]*)\)', re.MULTILINE | re.DOTALL)
        
        # 元数据解析模式
        self.meta_pattern = re.compile(r'meta\s*=\s*\((.*?)\)', re.DOTALL)
        self.meta_attr_pattern = re.compile(r'(\w+)\s*=\s*"([^"]*)"')
        
    def log(self, phase: ReflectionPhase, message: str):
        """记录日志"""
        if self.verbose:
            print(f"[{phase.value}] {message}")
    
    def parse_metadata(self, meta_str: str) -> Dict[str, str]:
        """解析元数据字符串"""
        metadata = {}
        
        # 提取meta=(...)部分
        meta_match = self.meta_pattern.search(meta_str)
        if meta_match:
            meta_content = meta_match.group(1)
            # 解析key="value"格式的属性
            for match in self.meta_attr_pattern.finditer(meta_content):
                key, value = match.groups()
                metadata[key] = value
        
        return metadata
    
    def preprocess_files(self) -> List[Path]:
        """预处理阶段：收集所有需要处理的头文件"""
        if self.single_file_mode:
            # 单文件模式：只处理指定的文件
            single_file = Path(self.single_file_mode)
            if not single_file.exists():
                self.log(ReflectionPhase.PREPROCESS, f"指定的文件不存在: {single_file}")
                return []
            
            # 检查单个文件是否需要反射处理
            try:
                content = single_file.read_text(encoding='utf-8')
                if any(pattern in content for pattern in ["JZCLASS", "JZPROPERTY", "JZMETHOD", "JZREFLECTION_FILE"]):
                    self.log(ReflectionPhase.PREPROCESS, f"单文件模式处理: {single_file}")
                    return [single_file]
                else:
                    self.log(ReflectionPhase.PREPROCESS, f"单文件无反射宏: {single_file}")
                    return []
            except Exception as e:
                self.log(ReflectionPhase.PREPROCESS, f"读取单文件失败 {single_file}: {e}")
                return []
        
        # 常规模式：扫描所有文件
        self.log(ReflectionPhase.PREPROCESS, f"扫描源文件目录: {self.source_dir}")
        
        header_files = []
        
        # 收集所有.h文件
        for header_file in self.source_dir.rglob("*.h"):
            # 跳过生成的文件
            if ".generated." in header_file.name:
                continue
                
            # 检查文件是否包含反射宏
            try:
                content = header_file.read_text(encoding='utf-8')
                if any(pattern in content for pattern in ["JZCLASS", "JZPROPERTY", "JZMETHOD", "JZREFLECTION_FILE"]):
                    header_files.append(header_file)
                    self.log(ReflectionPhase.PREPROCESS, f"发现反射文件: {header_file}")
            except Exception as e:
                self.log(ReflectionPhase.PREPROCESS, f"读取文件失败 {header_file}: {e}")
        
        self.log(ReflectionPhase.PREPROCESS, f"找到 {len(header_files)} 个反射文件")
        return header_files
    
    def parse_with_regex(self, file_path: Path) -> Optional[ClassInfo]:
        """使用正则表达式解析文件（fallback方法）"""
        try:
            content = file_path.read_text(encoding='utf-8')
            
            # 查找JZCLASS宏
            class_matches = list(self.jzclass_pattern.finditer(content))
            if not class_matches:
                return None
            
            # 假设每个文件只有一个JZCLASS
            class_match = class_matches[0]
            
            # 查找类名（在JZCLASS后面的class声明）
            class_decl_pattern = re.compile(r'class\s+(\w+)(?:\s*:\s*public\s+(\w+))?', re.MULTILINE)
            class_start = class_match.end()
            class_decl_match = class_decl_pattern.search(content, class_start)
            
            if not class_decl_match:
                self.log(ReflectionPhase.PARSE, f"找不到类声明: {file_path}")
                return None
            
            class_name = class_decl_match.group(1)
            parent_name = class_decl_match.group(2) if class_decl_match.group(2) else ""
            
            # 解析类元数据
            class_metadata = self.parse_metadata(class_match.group(1))
            
            class_info = ClassInfo(
                name=class_name,
                parent_name=parent_name,
                display_name=class_metadata.get('DisplayName', class_name),
                category=class_metadata.get('Category', ''),
                file_path=str(file_path),
                line_number=content[:class_match.start()].count('\n') + 1
            )
            
            # 查找JZPROPERTY - 使用新的正则表达式解析
            for prop_match in self.jzproperty_pattern.finditer(content):
                # 提取元数据 - 修复嵌套括号问题
                full_match = prop_match.group(0)
                meta_match = re.search(r'JZPROPERTY\s*\((.*)\)(?=\s*\w)', full_match, re.DOTALL)
                prop_metadata = {}
                if meta_match:
                    prop_metadata = self.parse_metadata(meta_match.group(1))
                
                # 直接从正则表达式组中获取类型和名称
                prop_type = prop_match.group(1).strip()
                prop_name = prop_match.group(2).strip()
                
                prop_info = PropertyInfo(
                    name=prop_name,
                    type_name=prop_type,
                    display_name=prop_metadata.get('DisplayName', prop_name),
                    category=prop_metadata.get('Category', ''),
                    tooltip=prop_metadata.get('Tooltip', ''),
                    file_path=str(file_path),
                    line_number=content[:prop_match.start()].count('\n') + 1
                )
                class_info.properties.append(prop_info)
            
            # 查找JZMETHOD - 使用新的正则表达式解析
            for method_match in self.jzmethod_pattern.finditer(content):
                # 提取元数据 - 修复嵌套括号问题
                full_match = method_match.group(0)
                meta_match = re.search(r'JZMETHOD\s*\((.*)\)(?=\s*\w)', full_match, re.DOTALL)
                method_metadata = {}
                if meta_match:
                    method_metadata = self.parse_metadata(meta_match.group(1))
                
                # 直接从正则表达式组中获取返回类型、方法名和参数
                return_type = method_match.group(1).strip()
                method_name = method_match.group(2).strip()
                params_str = method_match.group(3).strip()
                
                # 解析参数
                parameters = []
                if params_str:
                    for param in params_str.split(','):
                        param = param.strip()
                        if param:
                            param_parts = param.split()
                            if len(param_parts) >= 2:
                                param_type = ' '.join(param_parts[:-1])
                                param_name = param_parts[-1]
                                parameters.append((param_type, param_name))
                            elif len(param_parts) == 1:
                                # 仅有类型，无参数名
                                parameters.append((param, ""))
                
                method_info = MethodInfo(
                    name=method_name,
                    return_type=return_type,
                    parameters=parameters,
                    display_name=method_metadata.get('DisplayName', method_name),
                    category=method_metadata.get('Category', ''),
                    file_path=str(file_path),
                    line_number=content[:method_match.start()].count('\n') + 1
                )
                class_info.methods.append(method_info)
            
            return class_info
            
        except Exception as e:
            self.log(ReflectionPhase.PARSE, f"解析文件失败 {file_path}: {e}")
            return None
    
    def extract_macro_metadata(self, file_content: str, macro_start: int, macro_name: str) -> Dict[str, str]:
        """从宏调用中提取元数据参数"""
        metadata = {}
        
        # 找到宏的开始和结束位置
        paren_count = 0
        start_pos = file_content.find('(', macro_start)
        if start_pos == -1:
            return metadata
        
        end_pos = start_pos + 1
        while end_pos < len(file_content) and (paren_count > 0 or file_content[end_pos] != ')'):
            if file_content[end_pos] == '(':
                paren_count += 1
            elif file_content[end_pos] == ')':
                paren_count -= 1
            end_pos += 1
        
        if end_pos >= len(file_content):
            return metadata
        
        # 提取宏参数内容
        macro_content = file_content[start_pos + 1:end_pos]
        
        # 解析meta=(...)格式
        meta_match = self.meta_pattern.search(macro_content)
        if meta_match:
            meta_inner = meta_match.group(1)
            for attr_match in self.meta_attr_pattern.finditer(meta_inner):
                key, value = attr_match.groups()
                metadata[key] = value
        
        return metadata
    
    def visit_class_cursor(self, cursor, class_info: ClassInfo, file_content: str):
        """访问类定义的AST节点"""
        for child in cursor.get_children():
            # 检查是否是源文件中的定义（不是从include文件来的）
            if child.location.file and str(child.location.file) != class_info.file_path:
                continue
                
            if child.kind == CursorKind.FIELD_DECL:
                # 检查字段前是否有JZPROPERTY宏
                field_start_offset = child.extent.start.offset
                
                # 在字段前查找JZPROPERTY宏（向前查找最多1000个字符）
                search_start = max(0, field_start_offset - 1000)
                search_content = file_content[search_start:field_start_offset]
                
                # 查找最近的JZPROPERTY宏
                property_matches = list(self.jzproperty_pattern.finditer(search_content))
                if property_matches:
                    last_match = property_matches[-1]
                    # 检查宏和字段之间是否只有空白字符
                    between_content = search_content[last_match.end():].strip()
                    if not between_content or between_content.isspace():
                        # 提取宏的元数据
                        macro_start = search_start + last_match.start()
                        metadata = self.extract_macro_metadata(file_content, macro_start, "JZPROPERTY")
                        
                        # 创建属性信息
                        prop_info = PropertyInfo(
                            name=child.spelling,
                            type_name=child.type.spelling,
                            display_name=metadata.get('DisplayName', child.spelling),
                            category=metadata.get('Category', ''),
                            tooltip=metadata.get('Tooltip', ''),
                            is_public=(child.access_specifier == AccessSpecifier.PUBLIC),
                            file_path=class_info.file_path,
                            line_number=child.location.line
                        )
                        class_info.properties.append(prop_info)
                        
            elif child.kind == CursorKind.CXX_METHOD:
                # 检查方法前是否有JZMETHOD宏
                method_start_offset = child.extent.start.offset
                
                # 在方法前查找JZMETHOD宏（向前查找最多1000个字符）
                search_start = max(0, method_start_offset - 1000)
                search_content = file_content[search_start:method_start_offset]
                
                # 查找最近的JZMETHOD宏
                method_matches = list(self.jzmethod_pattern.finditer(search_content))
                if method_matches:
                    last_match = method_matches[-1]
                    # 检查宏和方法之间是否只有空白字符或类型声明
                    between_content = search_content[last_match.end():].strip()
                    if not between_content or self._is_method_declaration_start(between_content):
                        # 提取宏的元数据
                        macro_start = search_start + last_match.start()
                        metadata = self.extract_macro_metadata(file_content, macro_start, "JZMETHOD")
                        
                        # 获取方法参数
                        parameters = []
                        for param in child.get_arguments():
                            param_type = param.type.spelling
                            param_name = param.spelling
                            parameters.append((param_type, param_name))
                        
                        # 创建方法信息
                        method_info = MethodInfo(
                            name=child.spelling,
                            return_type=child.result_type.spelling,
                            parameters=parameters,
                            display_name=metadata.get('DisplayName', child.spelling),
                            category=metadata.get('Category', ''),
                            is_public=(child.access_specifier == AccessSpecifier.PUBLIC),
                            file_path=class_info.file_path,
                            line_number=child.location.line
                        )
                        class_info.methods.append(method_info)
    
    def _is_method_declaration_start(self, content: str) -> bool:
        """检查内容是否看起来像方法声明的开始"""
        # 移除注释和多余空白
        content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
        content = re.sub(r'//.*$', '', content, flags=re.MULTILINE)
        content = content.strip()
        
        # 检查是否包含可能的返回类型关键字
        type_keywords = ['void', 'int', 'float', 'double', 'bool', 'char', 'std::', 'const', 'static', 'virtual', 'inline']
        return any(keyword in content for keyword in type_keywords)
    
    def parse_with_clang(self, file_path: Path) -> Optional[ClassInfo]:
        """使用libclang解析文件"""
        if not CLANG_AVAILABLE:
            return self.parse_with_regex(file_path)
        
        try:
            # 读取文件内容用于宏检查
            file_content = file_path.read_text(encoding='utf-8')
            
            # 检查文件是否包含反射宏
            if not any(macro in file_content for macro in ["JZCLASS", "JZPROPERTY", "JZMETHOD"]):
                return None
            
            # 创建index和翻译单元
            index = clang.cindex.Index.create()
            
            # 设置编译参数
            compile_args = [
                '-std=c++20',
                '-I' + str(self.source_dir),  # 添加源码目录到包含路径
                '-DJZREFLECTION_ENABLED=1',   # 定义反射宏启用标志
                '-x', 'c++',  # 明确指定为C++
                '-Wno-pragma-once-outside-header',  # 抑制一些警告
            ]
            
            # 解析文件
            translation_unit = index.parse(
                str(file_path), 
                args=compile_args,
                options=TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD
            )
            
            # 检查诊断信息
            has_errors = False
            for diag in translation_unit.diagnostics:
                if diag.severity >= clang.cindex.Diagnostic.Error:
                    has_errors = True
                    self.log(ReflectionPhase.PARSE, f"解析错误: {diag.spelling} at {diag.location}")
            
            # 如果有严重错误，回退到正则表达式解析
            if has_errors:
                self.log(ReflectionPhase.PARSE, f"Clang解析有错误，回退到正则表达式解析: {file_path}")
                return self.parse_with_regex(file_path)
            
            # 遍历AST查找类定义
            classes_found = []
            self._find_classes_recursive(translation_unit.cursor, file_path, file_content, classes_found)
            
            # 返回第一个找到的类（假设每个文件只有一个主要的反射类）
            if classes_found:
                return classes_found[0]
            else:
                self.log(ReflectionPhase.PARSE, f"未找到反射类定义: {file_path}")
                return self.parse_with_regex(file_path)  # 回退到正则表达式
                
        except Exception as e:
            self.log(ReflectionPhase.PARSE, f"Clang解析失败，使用正则表达式: {e}")
            return self.parse_with_regex(file_path)
    
    def _find_classes_recursive(self, cursor, file_path: Path, file_content: str, classes_found: List[ClassInfo]):
        """递归查找类定义"""
        for child in cursor.get_children():
            # 只处理当前文件中的节点
            if child.location.file and str(child.location.file) != str(file_path):
                continue
                
            if child.kind == CursorKind.CLASS_DECL:
                # 检查类前是否有JZCLASS宏
                class_start_offset = child.extent.start.offset
                
                # 在类定义前查找JZCLASS宏（向前查找最多2000个字符）
                search_start = max(0, class_start_offset - 2000)
                search_content = file_content[search_start:class_start_offset]
                
                # 查找JZCLASS宏
                class_matches = list(self.jzclass_pattern.finditer(search_content))
                if class_matches:
                    last_match = class_matches[-1]
                    
                    # 提取类的宏元数据
                    macro_start = search_start + last_match.start()
                    metadata = self.extract_macro_metadata(file_content, macro_start, "JZCLASS")
                    
                    # 获取基类信息
                    parent_name = ""
                    for base in child.get_children():
                        if base.kind == CursorKind.CXX_BASE_SPECIFIER:
                            parent_name = base.type.spelling
                            break
                    
                    # 创建类信息
                    class_info = ClassInfo(
                        name=child.spelling,
                        parent_name=parent_name,
                        display_name=metadata.get('DisplayName', child.spelling),
                        category=metadata.get('Category', ''),
                        file_path=str(file_path),
                        line_number=child.location.line,
                        namespace="JzRE"  # 可以从AST中提取，但暂时硬编码
                    )
                    
                    # 访问类的成员
                    self.visit_class_cursor(child, class_info, file_content)
                    
                    classes_found.append(class_info)
                    
                    self.log(ReflectionPhase.PARSE, 
                           f"通过AST找到反射类 {class_info.name}: {len(class_info.properties)} 属性, {len(class_info.methods)} 方法")
            
            # 递归处理子节点
            self._find_classes_recursive(child, file_path, file_content, classes_found)
    
    def extract_reflection_data(self, files: List[Path]):
        """提取阶段：收集类型信息、属性元数据和函数签名"""
        self.log(ReflectionPhase.EXTRACT, "开始提取反射数据")
        
        for file_path in files:
            self.log(ReflectionPhase.EXTRACT, f"处理文件: {file_path}")
            
            class_info = self.parse_with_clang(file_path)
            if class_info:
                self.classes[class_info.name] = class_info
                self.log(ReflectionPhase.EXTRACT, 
                        f"找到类 {class_info.name}: {len(class_info.properties)} 属性, {len(class_info.methods)} 方法")
            
            self.processed_files.add(str(file_path))
    
    def validate_reflection_data(self):
        """验证阶段：检查反射声明的一致性"""
        self.log(ReflectionPhase.VALIDATE, "开始验证反射数据")
        
        # 检查父类关系
        for class_name, class_info in self.classes.items():
            if class_info.parent_name and class_info.parent_name != "JzObject":
                if class_info.parent_name not in self.classes:
                    self.log(ReflectionPhase.VALIDATE, 
                            f"警告: 类 {class_name} 的父类 {class_info.parent_name} 未找到反射信息")
        
        # 检查属性类型
        for class_name, class_info in self.classes.items():
            for prop in class_info.properties:
                # 这里可以添加类型检查逻辑
                pass
        
        self.log(ReflectionPhase.VALIDATE, "验证完成")
    
    def generate_header_file(self, class_info: ClassInfo) -> str:
        """生成.generated.h文件内容"""
        header_content = f'''// 自动生成的反射代码 - 请勿手动编辑
#pragma once

#include "JzObject.h"
#include "JzReflectable.h"

namespace {class_info.namespace} {{

// 类 {class_info.name} 的反射信息声明
class {class_info.name}ReflectionData {{
public:
    static void RegisterReflectionData();
    static JzClass* GetStaticClass();
}};

}} // namespace {class_info.namespace}

// 类的反射声明宏实现
#define IMPLEMENT_{class_info.name.upper()}_REFLECTION() \\
    JzRE::JzClass* {class_info.namespace}::{class_info.name}::s_StaticClass = nullptr; \\
    JzRE::JzClass* {class_info.namespace}::{class_info.name}::StaticClass() {{ \\
        if (!s_StaticClass) {{ \\
            s_StaticClass = {class_info.namespace}::{class_info.name}ReflectionData::GetStaticClass(); \\
        }} \\
        return s_StaticClass; \\
    }} \\
    JzRE::JzClass* {class_info.namespace}::{class_info.name}::GetClass() const {{ \\
        return StaticClass(); \\
    }}
'''
        return header_content
    
    def generate_source_file(self, class_info: ClassInfo) -> str:
        """生成.generated.cpp文件内容"""
        source_content = f'''// 自动生成的反射代码 - 请勿手动编辑
#include "{Path(class_info.file_path).stem}.generated.h"
#include "{Path(class_info.file_path).name}"

namespace {class_info.namespace} {{

// 静态反射数据
static JzRE::JzClass* s_{class_info.name}Class = nullptr;

void {class_info.name}ReflectionData::RegisterReflectionData() {{
    if (s_{class_info.name}Class) {{
        return; // Already registered
    }}
    
    // 创建类信息
    auto classInfo = std::make_unique<JzRE::JzClass>("{class_info.name}", "{class_info.parent_name}");
    
'''
        
        # 注册属性
        for prop in class_info.properties:
            # 确保元数据信息完整
            display_name = prop.display_name if prop.display_name else prop.name
            category = prop.category if prop.category else ""
            tooltip = prop.tooltip if prop.tooltip else ""
            metadata_code = f'JzRE::JzPropertyMetadata("{prop.name}", "{display_name}", "{category}", "{tooltip}")'
            source_content += f'''    // 注册属性: {prop.name}
    {{
        auto accessor = std::make_unique<JzRE::JzTypedPropertyAccessor<{class_info.name}, {prop.type_name}>>(&{class_info.name}::{prop.name});
        auto property = std::make_unique<JzRE::JzProperty>("{prop.name}", std::move(accessor), {metadata_code});
        classInfo->AddProperty(std::move(property));
    }}
    
'''
        
        # 注册方法
        for method in class_info.methods:
            param_types = ', '.join(f'"{param[0]}"' for param in method.parameters)
            source_content += f'''    // 注册方法: {method.name}
    {{
        std::vector<String> paramTypes = {{{param_types}}};
        auto method = std::make_unique<JzRE::JzMethod>("{method.name}", "{method.return_type}", paramTypes);
        classInfo->AddMethod(std::move(method));
    }}
    
'''
        
        source_content += f'''    // 保存类信息并注册到全局注册表
    s_{class_info.name}Class = classInfo.get();
    JzRE::JzReflectionRegistry::Get().RegisterClass(std::move(classInfo));
}}

JzRE::JzClass* {class_info.name}ReflectionData::GetStaticClass() {{
    if (!s_{class_info.name}Class) {{
        RegisterReflectionData();
    }}
    return s_{class_info.name}Class;
}}

// 自动注册器，确保在程序启动时注册反射信息
struct {class_info.name}AutoRegistrar {{
    {class_info.name}AutoRegistrar() {{
        {class_info.name}ReflectionData::RegisterReflectionData();
    }}
}};

static {class_info.name}AutoRegistrar s_{class_info.name}AutoRegistrar;

}} // namespace {class_info.namespace}

// 实现反射声明宏
IMPLEMENT_{class_info.name.upper()}_REFLECTION()
'''
        
        return source_content
    
    def _needs_regeneration(self, input_file: Path, output_files: List[Path]) -> bool:
        """检查是否需要重新生成文件"""
        if self.force_regenerate:
            return True
            
        # 检查输出文件是否存在
        for output_file in output_files:
            if not output_file.exists():
                return True
        
        # 检查输入文件是否比输出文件更新
        try:
            input_mtime = input_file.stat().st_mtime
            for output_file in output_files:
                output_mtime = output_file.stat().st_mtime
                if input_mtime > output_mtime:
                    return True
        except OSError:
            return True  # 如果无法获取文件时间，则重新生成
        
        return False
    
    def generate_code_files(self):
        """生成阶段：产生.generated.h和.generated.cpp文件"""
        self.log(ReflectionPhase.GENERATE, "开始生成代码文件")
        
        # 确保输出目录存在
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        generated_count = 0
        skipped_count = 0
        
        for class_name, class_info in self.classes.items():
            # 确定输出文件路径
            header_file = self.output_dir / f"{Path(class_info.file_path).stem}.generated.h"
            source_file = self.output_dir / f"{Path(class_info.file_path).stem}.generated.cpp"
            input_file = Path(class_info.file_path)
            
            # 检查是否需要重新生成
            if not self._needs_regeneration(input_file, [header_file, source_file]):
                self.log(ReflectionPhase.GENERATE, f"跳过生成 {class_name}（文件已是最新）")
                skipped_count += 1
                continue
            
            # 生成.generated.h文件
            header_content = self.generate_header_file(class_info)
            header_file.write_text(header_content, encoding='utf-8')
            self.log(ReflectionPhase.GENERATE, f"生成头文件: {header_file}")
            
            # 生成.generated.cpp文件
            source_content = self.generate_source_file(class_info)
            source_file.write_text(source_content, encoding='utf-8')
            self.log(ReflectionPhase.GENERATE, f"生成源文件: {source_file}")
            
            generated_count += 1
        
        if generated_count > 0:
            self.log(ReflectionPhase.GENERATE, f"代码生成完成，生成 {generated_count} 个类的反射代码")
        if skipped_count > 0:
            self.log(ReflectionPhase.GENERATE, f"跳过 {skipped_count} 个已是最新的类")
        
        if generated_count == 0 and skipped_count == 0:
            self.log(ReflectionPhase.GENERATE, "没有找到需要生成反射代码的类")
    
    def run(self):
        """运行反射工具的完整流程"""
        try:
            # 1. 预处理阶段
            files = self.preprocess_files()
            if not files:
                self.log(ReflectionPhase.PREPROCESS, "未找到需要处理的反射文件")
                return
            
            # 2. 解析和提取阶段
            self.extract_reflection_data(files)
            
            # 3. 验证阶段
            self.validate_reflection_data()
            
            # 4. 生成阶段
            self.generate_code_files()
            
            if self.verbose:
                print(f"\n反射工具执行完成!")
                print(f"处理了 {len(self.processed_files)} 个文件")
                print(f"生成了 {len(self.classes)} 个类的反射代码")
            elif len(self.classes) > 0:
                # 非详细模式只在有实际生成时输出
                print(f"反射工具完成，生成了 {len(self.classes)} 个类的反射代码")
            
        except Exception as e:
            print(f"反射工具执行失败: {e}")
            sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description='JzRE C++反射系统代码生成工具')
    parser.add_argument('--source-dir', '-s', required=True, help='源代码目录')
    parser.add_argument('--output-dir', '-o', required=True, help='输出目录')
    parser.add_argument('--single-file', '-f', help='只处理指定的单个文件（用于增量构建）')
    parser.add_argument('--verbose', '-v', action='store_true', help='详细输出')
    parser.add_argument('--force', action='store_true', help='强制重新生成，即使输出文件较新')
    
    args = parser.parse_args()
    
    if args.verbose:
        print("JzRE反射工具启动...")
        print(f"源码目录: {args.source_dir}")
        print(f"输出目录: {args.output_dir}")
        if args.single_file:
            print(f"单文件模式: {args.single_file}")
        if args.force:
            print("强制模式: 将重新生成所有文件")
    
    reflection_parser = ReflectionParser(args.source_dir, args.output_dir)
    
    # 设置选项
    reflection_parser.verbose = args.verbose
    reflection_parser.force_regenerate = args.force
    reflection_parser.single_file_mode = args.single_file
    
    reflection_parser.run()

if __name__ == "__main__":
    main()
