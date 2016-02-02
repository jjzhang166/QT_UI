
var TableFieldProtobuffCfg = {
    'player': {},
    'task': {'datas': 'TaskProtocol'},
    'skill': {'datas': 'SkillProtocol'},
    'item': {'datas': 'PBItemGroup'},
    'achieveevent': {'datas': 'AchieveEventProtocol'},
    'activity':{'datas': 'ActivityProtocol'},
    'rolepvp':{'datas': 'PvpProtocol'},
    'stall':{'datas': 'StallProtocol'},
    'trade':{'datas': 'TradeProtocol'},
    'common':{'datas': 'CommonProtocol'},
    'buff':{'datas': 'BuffProtocol'},
    'digmine':{'datas': 'DigMineProtocol'},
    'ride':{'datas': 'RideProtocol'},
    'achieve':{'datas': 'AchieveProtocol'},
    'title':{'datas': 'TitleProtocol'},
    'adore':{'datas': 'AdoreProtocol'},
    'relation_flowerrd':{'datas': 'RelationFlowerProtocol'},
}
var cachePbcData = {}; //!table -> field -> dict

function getPBFieldProtoName(tableName, field){
    if (TableFieldProtobuffCfg.hasOwnProperty(tableName)){
        var l = TableFieldProtobuffCfg[tableName];
        if (l.hasOwnProperty(field)){
            return l[field];
        }
    }
    return '';
}
function charsetConvert(data){
    return data;
    return QT.charestDecode(data);
}
function data2table(tableName, dbRet){
    var html = '';
    for (var i = 0; i < dbRet.result.length; ++i){
        var row = dbRet.result[i];
        var ret = '<table class="table table-bordered"><tbody>';
        for (var j = 0; j < dbRet.fieldNames.length; ++j){
            var fieldName = dbRet.fieldNames[j];
            if (j == 0)
                ret += '<tr><td>' + fieldName + '<input id="checkboxRow_'+i+'" type="checkbox" checked>'+'</td>';
            else
                ret += '<tr><td>' + fieldName +'</td>';
            var pbName = getPBFieldProtoName(tableName, fieldName);
            if (pbName == ''){
                //ret += '<td>' + charsetConvert(row[j]) + '</td></tr>';
                var idName = 'Edit-'+i+'-' + tableName + '-' + fieldName;
                ret += '<td><input  type="text" class="form-control" id="'+idName+'" placeholder="" value="'+charsetConvert(row[j])+'"></td></tr>';
                
            }
            else{
                var ret2 = row[j];
                var data = QT.pbcToJs("dbbuff.pb", pbName, ret2);
                console.log('pbcToJs', data);
                if (!cachePbcData.hasOwnProperty(tableName)){
                    cachePbcData[tableName] = {};
                }
                cachePbcData[tableName][fieldName] = data;
                var childTable = pb2table(data, 0, 'EditPBC');
                ret += '<td>' + childTable + '</td></tr>';
            }
        }
        ret += '</tr></tbody></table>'
        html += ret;
    }
    return html;
}

function pb2table(pbdata, level, preIdName){
    var html = '';//JSON.stringify(pbdata);
    
    var ret = '<table class="table table-bordered"><tbody>';
    for (var fieldName in pbdata){
        
        //console.log('pb2table', fieldName, pbdata[fieldName]);
        var fieldData = pbdata[fieldName][0];
        ret += '<tr><td>' + fieldName + '</td>';
        var idName = preIdName +'-'+ level + '-' + fieldName;
        
        if (typeof fieldData === 'object'){
            ret += '<td>' + pb2table(fieldData, level + 1, idName) + '</td></tr>';
        }
        else{
            //ret += '<td>' + charsetConvert(fieldData) + '</td></tr>';
            //console.log('pb2table idName', idName);
            ret += '<td><input  type="text" class="form-control" id="'+idName+'" placeholder="" value="'+ charsetConvert(fieldData) +'"></td></tr>';
        }
        
    }
    ret += '</tr></tbody></table>'
    html += ret;
    return html;
}