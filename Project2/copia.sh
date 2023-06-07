dpath = /arquivos/antes/*

for FILE in $dpath
do 
if [[ -f $FILE ]]
then 
    cp $FILE
else
    echo "errou o caminho de dados"
fi 
done