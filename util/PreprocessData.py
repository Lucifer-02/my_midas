import pandas as pd
import polars as pl


def darpa_original(path) -> None:
    # header is source, destination, timestamp, attack
    df = pd.read_csv(
        path, names=['src', 'dst', 'ts', 'atk'],  dtype='category')

    # create new dataframe for attack column with no header
    atk_df = pd.DataFrame(columns=['is_atk'])

    # create new column for attack column, only 1 if attack, 0 if not
    atk_df['is_atk'] = df['atk'].apply(lambda x: 1 if x != '-' else 0)

    # create new dataframe for src and dst  and timestamp
    src_dst_df = pd.DataFrame(columns=['src', 'dst', 'ts'])

    # src and dst are ip addresses, so we need to convert them to int using hash function
    src_dst_df['src'] = df['src'].apply(lambda x: int(x.replace('.', '')))
    src_dst_df['dst'] = df['dst'].apply(lambda x: int(x.replace('.', '')))

    # the timestamp is 'DD/MM/YYYY-HH:MM' so we need to convert it to int but hour based
    #  src_dst_df['ts'] = df['ts'].apply(lambda x: (x.split('-')[0])).astype('category').cat.codes + 1
    src_dst_df['ts'] = df['ts'].apply(lambda x: (
        x.split(':')[0])).astype('category').cat.codes + 1
    #  src_dst_df['ts'] = pd.factorize(df['ts'])[0] + 1

    #  dates = pd.to_datetime(df['ts'])
    #  date_to_int = dates.factorize()[0] + 1
    #
    #  src_dst_df['ts'] = date_to_int
    #
    #  print(date_to_int)

    print(src_dst_df)

    # save the new dataframe to a csv file
    src_dst_df.to_csv('data/darpa2/Data.csv',
                      index=False, header=False)
    atk_df.to_csv('data/darpa2/Label.csv',
                  index=False, header=False)
    # write size of the data to a file
    with open('data/darpa2/Meta.txt', 'w') as f:
        f.write(str(len(df)))

# do the same but use polars instead of pandas


def darpa_original_polars(path) -> None:
    df = pl.read_csv(path, has_header=False)
    df.columns = ['src', 'dst', 'ts', 'atk']

    # create new dataframe for attack column with no header
    atk_df = pl.DataFrame(
        {'is_atk': df['atk'].apply(lambda x: 1 if x != '-' else 0)})

    # create new dataframe for src and dst  and timestamp
    src_dst_df = pl.DataFrame({
        'src': df['src'].apply(lambda x: int(x.replace('.', ''))),
        'dst': df['dst'].apply(lambda x: int(x.replace('.', ''))),
        'ts': df['ts'].rank(method='dense')
    })

    print(src_dst_df)

    #  tsm = df['ts'].rank(method='dense')
    #  print(tsm.head(10))

    # save the new dataframe to a csv file
    src_dst_df.write_csv('data/darpa2/Data.csv',
                         has_header=False)
    atk_df.write_csv('data/darpa2/Label.csv',
                     has_header=False)


if __name__ == '__main__':
    darpa_original('data/darpa2/darpa_original.csv')
    #  darpa_original_polars('data/DARPA/darpa_original.csv')
