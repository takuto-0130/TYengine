
$projectDir = Get-Item "$PSScriptRoot\.." | Select-Object -ExpandProperty FullName
$vcxprojPath = Join-Path $projectDir "TYengine.vcxproj"
$filtersPath = Join-Path $projectDir "TYengine.vcxproj.filters"

function New-GuidString {
    return [Guid]::NewGuid().ToString("D")
}

# 1. Read vcxproj
if (-not (Test-Path $vcxprojPath)) {
    Write-Error "Project file not found: $vcxprojPath"
    exit 1
}

[xml]$vcxproj = Get-Content $vcxprojPath
$ns = @{ msbuild = "http://schemas.microsoft.com/developer/msbuild/2003" }

$files = @()

$files = @()

$clCompileNodes = Select-Xml -Xml $vcxproj -XPath "//msbuild:ClCompile" -Namespace $ns
if ($clCompileNodes) {
    $files += $clCompileNodes | ForEach-Object {
        if ($_.Node.Include) {
            [PSCustomObject]@{
                Path = $_.Node.Include
                Type = "ClCompile"
            }
        }
    }
}

$clIncludeNodes = Select-Xml -Xml $vcxproj -XPath "//msbuild:ClInclude" -Namespace $ns
if ($clIncludeNodes) {
    $files += $clIncludeNodes | ForEach-Object {
        if ($_.Node.Include) {
            [PSCustomObject]@{
                Path = $_.Node.Include
                Type = "ClInclude"
            }
        }
    }
}

$textNodes = Select-Xml -Xml $vcxproj -XPath "//msbuild:Text" -Namespace $ns
if ($textNodes) {
    $files += $textNodes | ForEach-Object {
        if ($_.Node.Include) {
            [PSCustomObject]@{
                Path = $_.Node.Include
                Type = "Text"
            }
        }
    }
}

Write-Host "Found $($files.Count) files."

# 2. Build Filter Structure
$filters = @{}
$itemsWithFilters = @()

foreach ($file in $files) {
    $normalizedPath = $file.Path -replace "/", "\"
    $dirName = [System.IO.Path]::GetDirectoryName($normalizedPath)
    
    $filterName = ""
    if (-not [string]::IsNullOrEmpty($dirName)) {
        $filterName = $dirName
        
        # Add parent filters
        $parts = $filterName -split "\\"
        $currentPath = ""
        foreach ($part in $parts) {
            if ($currentPath) {
                $currentPath += "\" + $part
            } else {
                $currentPath = $part
            }
            
            if (-not $filters.ContainsKey($currentPath)) {
                $filters[$currentPath] = New-GuidString
            }
        }
    }
    
    $itemsWithFilters += [PSCustomObject]@{
        Path = $normalizedPath
        Type = $file.Type
        Filter = $filterName
    }
}

# 3. Create new Filters XML
$xmlDoc = New-Object System.Xml.XmlDocument
$projectNode = $xmlDoc.CreateElement("Project", "http://schemas.microsoft.com/developer/msbuild/2003")
$projectNode.SetAttribute("ToolsVersion", "4.0")
$xmlDoc.AppendChild($projectNode) | Out-Null

# Filters ItemGroup
$filtersItemGroup = $xmlDoc.CreateElement("ItemGroup", "http://schemas.microsoft.com/developer/msbuild/2003")
$projectNode.AppendChild($filtersItemGroup) | Out-Null

$sortedFilters = $filters.Keys | Sort-Object
foreach ($filterPath in $sortedFilters) {
    if ([string]::IsNullOrWhiteSpace($filterPath)) { continue }
    
    $filterElem = $xmlDoc.CreateElement("Filter", "http://schemas.microsoft.com/developer/msbuild/2003")
    $filterElem.SetAttribute("Include", $filterPath)
    
    $uidElem = $xmlDoc.CreateElement("UniqueIdentifier", "http://schemas.microsoft.com/developer/msbuild/2003")
    $uidElem.InnerText = "{" + $filters[$filterPath] + "}"
    $filterElem.AppendChild($uidElem) | Out-Null
    
    $filtersItemGroup.AppendChild($filterElem) | Out-Null
}

# Items ItemGroups
$clCompileGroup = $xmlDoc.CreateElement("ItemGroup", "http://schemas.microsoft.com/developer/msbuild/2003")
$clIncludeGroup = $xmlDoc.CreateElement("ItemGroup", "http://schemas.microsoft.com/developer/msbuild/2003")
$textGroup = $xmlDoc.CreateElement("ItemGroup", "http://schemas.microsoft.com/developer/msbuild/2003")

# We adding groups only if they have items, but usually it's better to add them even if empty or check.
# Let's add them all to project and populate.
$projectNode.AppendChild($clCompileGroup) | Out-Null
$projectNode.AppendChild($clIncludeGroup) | Out-Null
$projectNode.AppendChild($textGroup) | Out-Null

foreach ($item in $itemsWithFilters) {
    $parentGroup = $null
    if ($item.Type -eq "ClCompile") { $parentGroup = $clCompileGroup }
    elseif ($item.Type -eq "ClInclude") { $parentGroup = $clIncludeGroup }
    elseif ($item.Type -eq "Text") { $parentGroup = $textGroup }
    
    if ($parentGroup) {
        $itemElem = $xmlDoc.CreateElement($item.Type, "http://schemas.microsoft.com/developer/msbuild/2003")
        $itemElem.SetAttribute("Include", $item.Path)
        
         if (-not [string]::IsNullOrEmpty($item.Filter)) {
            $filterElem = $xmlDoc.CreateElement("Filter", "http://schemas.microsoft.com/developer/msbuild/2003")
            $filterElem.InnerText = $item.Filter
            $itemElem.AppendChild($filterElem) | Out-Null
         }
         
         $parentGroup.AppendChild($itemElem) | Out-Null
    }
}

# Save
$xmlWriterSettings = New-Object System.Xml.XmlWriterSettings
$xmlWriterSettings.Indent = $true
$xmlWriterSettings.Encoding = [System.Text.Encoding]::UTF8
$xmlWriter = [System.Xml.XmlWriter]::Create($filtersPath, $xmlWriterSettings)
$xmlDoc.Save($xmlWriter)
$xmlWriter.Close()

Write-Host "Generated $filtersPath"
