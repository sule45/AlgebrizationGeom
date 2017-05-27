$testOutput = Get-Content test_input.txt | .\program.exe
$testOutput > test_output.txt
$expectedOutput = Get-Content .\test_expectedOuptut.txt
for($i = 0; $i -le $testOutput.Count; $i++){
    if($testOutput[$i] -eq $expectedOutput[$i]){
        write-host "Test $i succeeded"
    }
    else{
        Write-Host "Test $i failed : "
        write-host "output:   " $testOutput[$i]
        write-host "expected: " $expectedOutput[$i]
    }
}
