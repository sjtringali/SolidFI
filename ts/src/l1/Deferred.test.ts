// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { describe, it } from 'node:test';
import assert from 'node:assert/strict';

import { Deferred } from './Deferred';
import { Converter } from './Converter';
import { Chain } from './Chain';

class UpperCase implements Converter<string, string> {
    resolve(value: string): string {
        return value.toUpperCase();
    }
}

describe('Deferred', () => {
    it('constructs the converter on first acquire()', () => {
        let constructions = 0;
        const d = new Deferred<string, string>(() => { constructions++; return new UpperCase(); });
        assert.equal(constructions, 0);
        d.acquire('hello', {});
        assert.equal(constructions, 1);
    });

    it('reuses the same converter instance on subsequent calls', () => {
        let constructions = 0;
        const d = new Deferred<string, string>(() => { constructions++; return new UpperCase(); });
        d.resolve('a', {});
        d.resolve('b', {});
        d.resolve('c', {});
        assert.equal(constructions, 1);
    });

    it('delegates resolve() to the constructed converter', () => {
        const d = new Deferred<string, string>(() => new UpperCase());
        assert.equal(d.resolve('hello', {}), 'HELLO');
        assert.equal(d.resolve('world', {}), 'WORLD');
    });

    it('is assignable to Converter<T, U>', () => {
        const d: Converter<string, string> = new Deferred(() => new UpperCase());
        assert.equal(d.resolve('hi', {}), 'HI');
    });

    it('can be installed in a Chain instead of a direct instance', () => {
        let constructions = 0;
        const chain = new Chain<string, string | null>(null);
        chain.install(1, 'upper', new Deferred(() => { constructions++; return new UpperCase(); }));

        assert.equal(constructions, 0);
        assert.equal(chain.resolve('hello', {}), 'HELLO');
        assert.equal(constructions, 1);
        assert.equal(chain.resolve('world', {}), 'WORLD');
        assert.equal(constructions, 1);
    });
});
